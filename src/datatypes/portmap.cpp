#include "portmap.hpp"

namespace dt {

std::string PortMap::pretty() const {
    std::string out;
    if (from) {
        out += std::to_string(*from);
        out += "->";
    }
    else {
        out += "[no nat]";
    }

    out += std::to_string(to);
    out += "/";
    out += proto;

    return out;
}

} // namespace