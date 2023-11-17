#pragma once

#include <string>
#include <vector>

class IPv4 {

public:
    const std::string orig;
    const std::vector<std::string> groups;

    IPv4(const std::string& orig_,
         const std::vector<std::string>& groups_)
            : orig(orig_)
            , groups(groups_)
    { }

};
