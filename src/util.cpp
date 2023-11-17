#include "util.hpp"

#include <algorithm>
#include <sstream>

namespace util {

bool is_number(const std::string& str) {
    return std::all_of(
            str.begin(),
            str.end(),
            [](unsigned char c){ return std::isdigit(c); });
}

bool is_hex_number(const std::string& str) {
    return std::all_of(
            str.begin(),
            str.end(),
            [](unsigned char c){ return std::isxdigit(c); });
}

std::vector<std::string> to_groups(const std::string& in, char delim,
                                   const std::string& error_tag,
                                   int group_count_min, int group_count_max,
                                   verify_group_fn verify) {
    std::stringstream inss(in);
    std::string group;
    int group_idx = 0;
    std::vector<std::string> groups;
    while (getline(inss, group, delim)) {
//        if (group.length() < group_size_min || group.length() > group_size_max)
//            goto invalid;
//        if (hex && !util::is_hex_number(group))
//            goto invalid;
//        if (number && !util::is_number(group))
//            goto invalid;

        if (!verify(group_idx, group))
            goto invalid;

        groups.push_back(group);
        group_idx++;
    }

    if (groups.size() < group_count_min || groups.size() > group_count_max)
        goto invalid;

    return groups;

invalid:
    throw parse_error("invalid " + error_tag + " group parsing: " + in);
}

std::string zfill(const std::string& str, size_t to) {
    std::string copy(str);
    while (copy.length() < to) {
        copy.insert(0, "0");
    }

    return copy;
}

}