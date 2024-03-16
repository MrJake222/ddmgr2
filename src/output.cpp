#include "output.hpp"

#include <flags.hpp>

namespace output {

namespace {
// private namespace

void dns_record(std::ostream& os, const std::string& name, int name_max_len,
                const std::string& record_type, const std::string& value) {
    // LAPTOP-CO9OHV8O A 192.168.1.179
    os << pad(name, name_max_len) << "    ";
    os << pad(record_type, 5)     << "    ";
    os << value;
    os << std::endl;
}

void dns(std::ostream& os, const Subnet& subnet, bool internal) {
    for (const Host& host : subnet.hosts) {

        for (const std::string& aname : host.anames()) {
            dns_record(os, aname, subnet.maxlen, "A",
                       internal
                           ? host.ipv4.orig
                           : subnet.v4_external);

            if (host.ipv6)
                dns_record(os, aname, subnet.maxlen, "AAAA", host.ipv6->orig);
        }

        for (const std::string& cname : host.cname) {
            dns_record(os, cname, subnet.maxlen, "CNAME", host.name);
        }

        if (host.ns) {
            dns_record(os, host.ns->for_domain, subnet.maxlen,
                       "NS", host.name);
        }

        if (host.mx) {
            dns_record(os, host.mx->for_domain, subnet.maxlen,
                       "MX " + std::to_string(host.mx->pri), host.name);
        }
    }
}

std::string pad(const std::string& str, int to) {
    std::string copy(str);
    while (copy.length() < to)
        copy += ' ';
    return copy;
}

} // end private namespace


void dhcp(std::ostream& os, const Subnet& subnet) {
    for (const Host& host : subnet.hosts) {
        if (!host.mac)
            continue;

        // host NorbertPC { hardware ethernet 4C:CC:6A:XX:XX:XX; fixed-address 192.168.1.2; }
        os << "host " << pad(host.name, subnet.maxlen) << " { ";
        os << "hardware ethernet " << host.mac->orig << "; ";
        os << "fixed-address " << host.ipv4.orig << "; ";
        os << "}" << std::endl;
    }
}

void dhcpv6(std::ostream& os, const Subnet& subnet) {
    for (const Host& host : subnet.hosts) {
        if (!host.mac)
            continue;
        if (!host.ipv6)
            continue;

        // host NorbertPC { hardware ethernet 4C:CC:6A:XX:XX:XX; fixed-address6 fd00::1; }
        os << "host " << pad(host.name, subnet.maxlen) << " { ";
        os << "hardware ethernet " << host.mac->orig << "; ";
        os << "fixed-address6 " << host.ipv6->orig << "; ";
        os << "}" << std::endl;
    }
}

void dnsext(std::ostream& os, const Subnet& subnet) {
    dns(os, subnet, false);
}

void dnsint(std::ostream& os, const Subnet& subnet) {
    dns(os, subnet, true);
}

void dnsrev(std::ostream& os, const Subnet& subnet) {
    // 1.1         PTR             c1.example.com.

    for (const Host& host : subnet.hosts) {
        dns_record(
                os,
                host.ipv4.ptr(FLAGS_dnsrev_mask), 0,
                "PTR",
                subnet.fqdn(host) + ".");
    }
}

void dnsrev6(std::ostream& os, const Subnet& subnet) {
    // 1.0.0.0.1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0         PTR             c1.example.com.

    for (const Host& host : subnet.hosts) {
        if (!host.ipv6)
            continue;

        dns_record(
                os,
                host.ipv6->ptr(FLAGS_dnsrev6_mask), 0,
                "PTR",
                subnet.fqdn(host) + ".");
    }
}

void nft(std::ostream& os, const Subnet& subnet) {
    for (const Host& host : subnet.hosts) {
        if (!host.ports.empty())
            os << "# host " << host.name << std::endl;

        for (const dt::PortMap& map : host.ports) {
            os << "# " << map.pretty() << std::endl;

            if (host.ipv6) {
                // ipv6 forward rule
                // add rule inet filter forward ip6 daddr XXXX:YYYY:ZZZZ:AAAA::1:1 tcp dport 22 accept
                os  << "add rule " << FLAGS_nft_chain_filter_forward << " "
                    << "ip6 daddr " << host.ipv6->orig << " "
                    << map.proto << " dport " << map.to << " "
                    << "accept" << std::endl;
            }

            // ipv4 forward rule
            // add rule inet filter forward ip daddr 172.16.1.1 tcp dport 22 accept
            os  << "add rule " << FLAGS_nft_chain_filter_forward << " "
                << "ip daddr " << host.ipv4.orig << " "
                << map.proto << " dport " << map.to << " "
                << "accept" << std::endl;

            if (map.from) {
                // ipv4 nat
                // add rule ip nat prerouting ip saddr != $INTERNAL tcp dport 222 dnat to 172.16.1.1:22
                os  << "add rule " << FLAGS_nft_chain_nat_prerouting << " "
                    << "ip saddr != " << FLAGS_nft_net_internal << " "
                    << map.proto << " dport " << *map.from << " "
                    << "dnat to " << host.ipv4.orig << ":" << map.to
                    << std::endl;
            }
        }

        if (!host.ports.empty())
            os << std::endl;
    }
}

} // namespace