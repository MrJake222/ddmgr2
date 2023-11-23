#pragma once

#include <string>

namespace dt {

class NS {

public:
    const std::string for_domain;

    NS(const std::string& for_domain_)
            : for_domain(for_domain_)
    { }
};

} // namespace
