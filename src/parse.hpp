#pragma once

// Parsing and verifying should be contained here and only here
// We don't want to pollute data classes with obscure methods
// After parsing, data is immutable

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>
#include <exception>

#include <subnet.hpp>

#include <mac.hpp>
#include <ipv4.hpp>
#include <ipv6.hpp>
#include <portmap.hpp>

namespace parse {

std::string checked_convert(const YAML::Node& conf, const std::string& name, const std::string& error_suffix);

Subnet yaml_to_subnet(YAML::Node conf);
MAC parse_mac(const std::string& host, const std::string& in);
IPv4 parse_ipv4(const std::string& host, const std::string& in);
IPv6 parse_ipv6(const std::string& host, const std::string& in);
std::vector<PortMap> parse_ports(const std::string& host, const std::vector<std::string>& ports);

void print_host(const Host& host);

} // namespace