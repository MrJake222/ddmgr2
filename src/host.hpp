#pragma once

#include <string>
#include <vector>

#include <optional>

#include <mac.hpp>
#include <ipv4.hpp>
#include <ipv6.hpp>
#include <portmap.hpp>

class Host {

public:
    const std::string name;
    const dt::MAC mac;
    const dt::IPv4 ipv4;
    const std::optional<dt::IPv6> ipv6;
    const std::vector<dt::PortMap> ports;
    const std::vector<std::string> aname;
    const std::vector<std::string> cname;
    const std::optional<int> mx;

    Host(const std::string& name_,
         const dt::MAC& mac_,
         const dt::IPv4& ipv4_,
         const std::optional<dt::IPv6>& ipv6_,
         const std::vector<dt::PortMap>& ports_,
         const std::vector<std::string>& aname_,
         const std::vector<std::string>& cname_,
         const std::optional<int>& mx_)
            : name(name_)
            , mac(mac_)
            , ipv4(ipv4_)
            , ipv6(ipv6_)
            , ports(ports_)
            , aname(aname_)
            , cname(cname_)
            , mx(mx_)
            { }
};
