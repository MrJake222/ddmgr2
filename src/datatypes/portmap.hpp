#pragma once

#include <string>

namespace dt {

class PortMap {

public:
    const std::string proto;
    const int from;
    const int to;

    PortMap(const std::string& proto_,
         const int from_,
         const int to_)
            : proto(proto_)
            , from(from_)
            , to(to_)
    { }
};

} // namespace