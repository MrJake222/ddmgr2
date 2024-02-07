#pragma once

#include <string>
#include <vector>

#include <optional>

#include <mac.hpp>
#include <ipv4.hpp>
#include <ipv6.hpp>
#include <portmap.hpp>
#include <ns.hpp>
#include <mx.hpp>

class Host {

    std::vector<std::string> anames_;

public:
    const std::string name;
    const dt::IPv4 ipv4;
    const std::optional<dt::MAC> mac;
    const std::optional<dt::IPv6> ipv6;
    const std::vector<dt::PortMap> ports;
    const std::vector<std::string> aname;
    const std::vector<std::string> cname;
    const std::optional<dt::NS> ns;
    const std::optional<dt::MX> mx;

    Host(const std::string& name_,
         const dt::IPv4& ipv4_,
         const std::optional<dt::MAC>& mac_,
         const std::optional<dt::IPv6>& ipv6_,
         const std::vector<dt::PortMap>& ports_,
         const std::vector<std::string>& aname_,
         const std::vector<std::string>& cname_,
         const std::optional<dt::NS>& ns_,
         const std::optional<dt::MX>& mx_)
            : name(name_)
            , ipv4(ipv4_)
            , mac(mac_)
            , ipv6(ipv6_)
            , ports(ports_)
            , aname(aname_)
            , cname(cname_)
            , ns(ns_)
            , mx(mx_)
            {
        anames_.emplace_back(name);
        anames_.insert(anames_.end(), aname.begin(), aname.end());
            }

    // returns name + aname records
    // useful for writing A/AAAA records
    std::vector<std::string> anames() const { return anames_; }
};
