#pragma once

#include <string>
#include <vector>

#include <host.hpp>

class Subnet {
//    const std::string& v4_prefix;
//    const std::string& v4_external;
//    const std::string& v6_prefix;
//    const std::string& domain;

    const std::vector<Host>& hosts;

public:
    Subnet(const std::vector<Host>& hosts_)
        : hosts(hosts_)
        { }
};