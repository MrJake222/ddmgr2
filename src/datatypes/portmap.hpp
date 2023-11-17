#pragma once

#include <string>
#include <optional>

namespace dt {

class PortMap {

public:
    const std::string proto;
    const std::optional<int> from;
    const int to;

    PortMap(const std::string& proto_,
         const std::optional<int> from_,
         const int to_)
            : proto(proto_)
            , from(from_)
            , to(to_)
    { }
};

} // namespace