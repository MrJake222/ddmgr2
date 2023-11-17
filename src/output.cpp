#include "output.hpp"

#include <flags.hpp>

namespace output {

namespace {
// private namespace

void dns_record(std::ostream& os, const std::string& name,
                const std::string& record_type, const std::string& value) {
    // LAPTOP-CO9OHV8O A 192.168.1.179
    os << name << " ";
    os << record_type << " ";
    os << value;
    os << std::endl;
}

void dns(std::ostream& os, const Subnet& subnet, bool internal) {
    for (const auto& host : subnet.hosts) {

        for (const auto& aname : host.anames()) {
            dns_record(os, aname, "A",
                       internal
                           ? host.ipv4.orig
                           : subnet.v4_external);

            if (host.ipv6)
                dns_record(os, aname, "AAAA", host.ipv6->orig);
        }

        for (const auto& cname : host.cname) {
            dns_record(os, cname, "CNAME", host.name);
        }
    }
}

} // end private namespace


void dhcp(std::ostream& os, const Subnet& subnet) {
    for (const auto& host : subnet.hosts) {
        // host NorbertPC { hardware ethernet 4C:CC:6A:XX:XX:XX; fixed-address 192.168.1.2; }
        os << "host " << host.name << " { ";
        os << "hardware ethernet " << host.mac.orig << "; ";
        os << "fixed-address " << host.ipv4.orig << "; ";
        os << "}" << std::endl;
    }
}

void dhcpv6(std::ostream& os, const Subnet& subnet) {
    for (const auto& host : subnet.hosts) {
        if (!host.ipv6)
            return;

        // host NorbertPC { hardware ethernet 4C:CC:6A:XX:XX:XX; fixed-address6 fd00::1; }
        os << "host " << host.name << " { ";
        os << "hardware ethernet " << host.mac.orig << "; ";
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

void dnsrev6(std::ostream& os, const Subnet& subnet) {
    // 1.0.0.0.1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0         PTR             c1.example.com.

    for (const auto& host : subnet.hosts) {
        if (!host.ipv6)
            return;

        dns_record(
                os,
                host.ipv6->ptr(64),
                "PTR",
                subnet.fqdn(host) + ".");
    }
}

void nft(std::ostream& os, const Subnet& subnet) {
    for (const auto& host : subnet.hosts) {
        for (const auto& map : host.ports) {

            if (host.ipv6) {
                // ipv6 forward rule
                // add rule inet filter forward ip6 daddr 2001:470:6166::1:1 tcp dport 22 accept
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
    }
}

} // namespace