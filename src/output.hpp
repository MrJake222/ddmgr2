#pragma once

#include <ostream>

#include <subnet.hpp>

namespace output {

namespace {
// private namespace

void dns_record(std::ostream& os, const std::string& name, int name_max_len,
                const std::string& record_type, const std::string& value);

void dns(std::ostream& os, const Subnet& subnet, bool internal);

std::string pad(const std::string& str, int to);

} // end private namespace

typedef void(*fptr)(std::ostream& os, const Subnet& subnet);

void dhcp(std::ostream& os, const Subnet& subnet);
void dhcpv6(std::ostream& os, const Subnet& subnet);
void dnsext(std::ostream& os, const Subnet& subnet);
void dnsint(std::ostream& os, const Subnet& subnet);
void dnsrev6(std::ostream& os, const Subnet& subnet);
void nft(std::ostream& os, const Subnet& subnet);

} // namespace