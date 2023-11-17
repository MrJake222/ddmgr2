#pragma once

#include <vector>
#include <string>
#include <stdexcept>

class parse_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

namespace util {

bool is_number(const std::string& str);
bool is_hex_number(const std::string& str);

typedef bool (*verify_group_fn)(int group_idx, const std::string& group);

std::vector<std::string> to_groups(const std::string& in, char delim,
                                   const std::string& error_tag,
                                   int group_count_min, int group_count_max,
                                   verify_group_fn verify);

std::string zfill(const std::string& str, size_t to);

}