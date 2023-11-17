#include "subnet.hpp"

std::string Subnet::fqdn(const Host& host) const {
    return host.name + "." + domain;
}
