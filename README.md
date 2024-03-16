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
* `--dnsrev <file>` -- reverse DNS zone for v4 addresses
* `--dnsrev6 <file>` -- reverse DNS zone for v6 addresses
* `--nft <file>` -- nftables rules

#### Reverse zone parameters
Values here are kind of tricky. It's the subnet mask, not how many bits will the entry contain.
For example, host: `172.16.5.12/16` with `--dnsrev_mask 16` will generate `12.5  PTR  f.q.d.n.` in reverse zone.
Default behaviour is to generate all octets/hextets. This way no subnet-specific `$ORIGIN` directives are needed in base files.
* `--dnsrev_mask <int>` -- affects how long `--dnsrev` results will be (`0` means full 32-bit entries, default `0`)
* `--dnsrev6_mask <int>` -- affects how long `--dnsrev6` results will be (`0` means full 128-bit entries, default `0`)

#### NFTables chains
* `--nft_chain_filter_forward <string>` -- nftables filter forward chain name (for firewall rules generation, default `FILTER FORWARD`)
* `--nft_chain_nat_prerouting <string>` -- nftables nat prerouting chain name (for nat rules generation, default `NAT PREROUTING`)
* `--nft_net_internal <string>` -- nftables internal networks variable (for dnat exclusion, default `$INTERNAL`)

#### Debug variables
* `--verbose` -- enables printing of debug messages (default `false`)

## Config file
Consists of common config directives:
* `v4-prefix` -- private v4 prefix to be used on local network (classful, private, no host octets, ex. `192.168`)
* `v4-external` -- public v4 address
* `v6-prefix` -- public v6 prefix to be used on local network (no `:`, ex. `2000`)
* `domain` -- domain name (ex. `home.local`)

Also contains `hosts` directive, which contains a list of hosts entries (keys don't matter).
Value must contain:
* `name` -- name (DNS compatible)
* `ipv4` -- v4 suffix (host octets with leading dot, ex. `.1.2`)

Additionally, user can specify:
* `mac` -- MAC address
* `ipv6` -- v6 suffix (host part, with leading `:` or `::`, ex. `::2`)
* `ports` -- list of port forward definitions, format: `proto:external:internal`, for ex: `tcp:25:25`
    * `proto` -- which protocol to use
    * `external` -- external port to use for dnat to this host internal port, can be omitted (no dnat, only firewall rules)
    * `internal` -- internal port
* `aname` -- list of names; will generate `A`/`AAAA` records with the same name as host, useful for `@`
* `cname` -- list of names; will generate `CNAME` records pointing to this host
* `ns` -- name server, will generate `NS` record for given domain (key `for`) poiting to this host
* `mx` -- mail exchange priority; will generate `MX` record for given domain (key `for`) with given priority (key `pri`) pointing to this host

Example input can be found in `examples/subnets.d/subnet.yaml`.

This version has more trust for the user and doesn't check anything ;)
(though it verifies MAC, IPv4, IPv6, port mappings compliance with standard)

## Provided scripting infrastructure
Scripting infra was placed in `script` directory.

#### `install.sh` -- Installation script
Can install/uninstall/update `ddmgr2`.

#### `ddmgr2.service` -- Installation script
Systemd service which can be used to generate config before any other system service using those files starts.

#### `ddmgr2_config.sh` -- Config generation helper
Multiple subnets are supported.
Subnets are located in (assuming `/usr/local` prefix): `/usr/local/etc/ddmgr2/subnets.d`.
Folder should contain differently named `.yaml` files, which are then sequentially passed to `ddmgr2`
to generate config and append it to global system files.
For the files to not grow indefinitely, the concept of base files is used.
They are read-only files that hold common config without auto-generated hosts entries.
Base files are used to copy over the generated content with unmodified versions.
