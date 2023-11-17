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


// --------------- helper functions ---------------

bool is_number(const std::string& str);
bool is_hex_number(const std::string& str);

typedef bool (*verify_group_fn)(int group_idx, const std::string& group);

std::vector<std::string> to_groups(const std::string& in, char delim,
                                   const std::string& error_tag,
                                   int group_count_min, int group_count_max,
                                   verify_group_fn verify);

std::string checked_as(const YAML::Node& conf, const std::string& name, const std::string& error_suffix);


// --------------- parsing functions ---------------

Subnet yaml_to_subnet(YAML::Node conf);
dt::MAC parse_mac(const std::string& host, const std::string& in);
dt::IPv4 parse_ipv4(const std::string& host, const std::string& in);
dt::IPv6 parse_ipv6(const std::string& host, const std::string& in);
std::vector<dt::PortMap> parse_ports(const std::string& host, const std::vector<std::string>& ports);


// --------------- printing functions (debug) ---------------

void print_host(const Host& host);

} // namespace