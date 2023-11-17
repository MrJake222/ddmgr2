#pragma once

#include <string>
#include <vector>

namespace dt {

class MAC {

public:
    const std::string orig;
    const std::vector<std::string> groups;

    MAC(const std::string& orig_,
        const std::vector<std::string>& groups_)
            : orig(orig_)
            , groups(groups_)
            { }
    
};

} // namespace