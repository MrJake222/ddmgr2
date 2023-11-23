#pragma once

#include <string>

namespace dt {

class MX {

public:
    const std::string for_domain;
    const int pri;

    MX(const std::string& for_domain_,
       int pri_)
            : for_domain(for_domain_)
            , pri(pri_)
    { }
};

} // namespace
