# DHCP-DNS-other (version 2.0) manager

This is a rewrite of my previous project [dhcp-dns-manager](https://github.com/MrJake222/dhcp_dns_manager).
What's new?
- YAML config instead of line-based proprietary config
- one subnet = one file (instead of one folder)
- one parsing, multiple output

Requires `yaml-cpp` and `gflags` to be installed on the system.

## Quick start
- Install cmake, make, gcc
    - Debian: `apt install cmake build-essential`
- Install gflags, yamlcpp
    - Debian: `apt install libgflags-dev libyaml-cpp-dev`
- `git clone https://github.com/MrJake222/ddmgr2.git`
- `cd ddmgr2/script`
- `MAKEOPTS=-j4 ./install.sh build` (adjust number of cores)
- `./install.sh install`
- Edit files at `/usr/local/etc/ddmgr2`
    - You may want to link config dir to regular `/etc`:
      `ln -s /usr/local/etc/ddmgr2 /etc`
- Edit `/etc/systemd/system/ddmgr2.service` to only be required by and
  start before installed services
- `systemctl enable ddmgr2 --now`
- If you don't want to use systemd daemon just run `/usr/local/bin/ddmgr2_config.sh`
  to regenerate config

## Program arguments
### Positional arguments
* `config.yaml` -- path to file containing host/firewall/other defininitons (format below, required)

### Flags
Flags are optional. Can be specified in any order. They change the behavior of the program.

#### Config generation
Can specify as many as required. Program will append appropriate configuration
to each specified file.
* `--dhcp <file>` -- ISC-DHCP server, v4
* `--dhcpv6 <file>` -- ISC-DHCP server, v6
* `--dnsint <file>` -- BIND9, internal view (private v4-prefix / v6-prefix)
* `--dntext <file>` -- BIND9, external view (public v4 (same for all hosts) / v6-prefix)
* `--dnsrev6 <file>` -- reverse DNS zone for v6 addresses
* `--nft <file>` -- nftables rules

#### Program variables
* `--verbose` -- enables printing of debug messages (default `false`)
* `--dnsrev6_mask <int>` -- affects how log `--dnsrev6` results will be (`0` means full 128-bit entries, default `0`)
* `--nft_chain_filter_forward <string>` -- nftables filter forward chain name (for firewall rules generation, default `FILTER FORWARD`)
* `--nft_chain_nat_prerouting <string>` -- nftables nat prerouting chain name (for nat rules generation, default `NAT PREROUTING`)
* `--nft_net_internal <string>` -- nftables internal networks variable (for dnat exclusion, default `$INTERNAL`)

## Config file
Consists of common config directives:
* `v4-prefix` -- private v4 prefix to be used on local network (classful, private, no host octets, ex. `192.168`)
* `v4-external` -- public v4 address
* `v6-prefix` -- public v6 prefix to be used on local network (no `:`, ex. `2000`)
* `domain` -- domain name (ex. `home.local`)

Also contains `hosts` directive, which contains a list of hosts entries (keys don't matter).
Value must contain:
* `name` -- name (DNS compatible)
* `mac` -- MAC address
* `ipv4` -- v4 suffix (host octets with leading dot, ex. `.1.2`)

Additionally user can specify:
* `ipv6` -- v6 suffix (host part, with leading `:` or `::`, ex. `::2`)
* `ports` -- list of port forward definitions, format: `proto:external:internal`, for ex: `tcp:25:25`
    * `proto` -- which protocol to use
    * `external` -- external port to use for dnat to this host internal port, can be omitted (no dnat, only firewall rules)
    * `internal` -- internal port
* `aname` -- list of names; will generate `A`/`AAAA` records with the same name as host, useful for `@`.
* `cname` -- list of names; will generate `CNAME` records pointing to this host
* `mx` -- mail exchange priority; will generate MX record for `@` pointing to this host

Example input can be found in `examples/subnet.yaml`.

This version has more trust for the user and doesn't check anything ;)
(though it verifies MAC, IPv4, IPv6, port mappings compliance with standard)

## Provided scripting infrastructure
Scripting infra was placed in `script` directory:
* `install.sh`
Generates config files for basic services. Base files are copied to output files
and to each output file script adds result of parsing subnet files.
Subnets are located in (assuming `/usr/local` prefix): `/usr/local/etc/ddmgr2/subnets.d`.
Folder should contain differently named `.yaml` files which define what was said above.
