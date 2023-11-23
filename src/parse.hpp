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
#include <ns.hpp>
#include <mx.hpp>

namespace parse {

class parse_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// --------------- helper functions ---------------
namespace {
// private namespace

bool is_number(const std::string& str);
bool is_hex_number(const std::string& str);
void zfill(std::string& str, size_t to);

typedef bool (*verify_group_fn)(int group_idx, const std::string& group);

std::vector<std::string> to_groups(const std::string& in, char delim,
                                   const std::string& error_tag,
                                   int group_count_min, int group_count_max,
                                   verify_group_fn verify);

std::string checked_as(const YAML::Node& conf, const std::string& name, const std::string& error_suffix);

// vector<string> to string
std::string vs2s(const std::vector<std::string>& v);

} // end private namespace

// --------------- parsing functions ---------------

Subnet yaml_to_subnet(const YAML::Node& conf);
dt::MAC parse_mac(const std::string& host, const std::string& in);
dt::IPv4 parse_ipv4(const std::string& host, const std::string& in);
dt::IPv6 parse_ipv6(const std::string& host, const std::string& in);
std::vector<dt::PortMap> parse_ports(const std::string& host, const std::vector<std::string>& ports);
dt::NS parse_ns(const std::string& host, const std::string& for_domain);
dt::MX parse_mx(const std::string& host, const std::string& for_domain, int pri);


// --------------- printing functions (debug) ---------------

void print_host(const Host& host);

} // namespace