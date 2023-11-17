#pragma once

#include <string>
#include <vector>

class IPv6 {

public:
    const std::string orig;
    const std::vector<std::string> groups;

    IPv6(const std::string& orig_,
        const std::vector<std::string>& groups_)
            : orig(orig_)
            , groups(groups_)
    { }

};