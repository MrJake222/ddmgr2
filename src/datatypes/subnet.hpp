#pragma once

#include <string>
#include <vector>

#include <host.hpp>

class Subnet {

public:
    const std::string v4_prefix;
    const std::string v4_external;
    const std::string v6_prefix;
    const std::string domain;
    const std::vector<Host> hosts;

    // maximum length across all hosts and all aliases
    const int maxlen;

    Subnet(const std::string& v4_prefix_,
           const std::string& v4_external_,
           const std::string& v6_prefix_,
           const std::string& domain_,
           const std::vector<Host>& hosts_,
           const int& host_name_len_max_)
        : v4_prefix(v4_prefix_)
        , v4_external(v4_external_)
        , v6_prefix(v6_prefix_)
        , domain(domain_)
        , hosts(hosts_)
        , maxlen(host_name_len_max_)
        { }

    std::string fqdn(const Host& host) const;
};