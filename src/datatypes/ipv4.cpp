#include "ipv4.hpp"

namespace dt {

std::string IPv4::ptr(int mask) const {
    std::string out;

    for (int i=3; i>=0; i--) {
        out += groups[i];
        out += ".";

        if (i*8 == mask)
            goto end;
    }

end:
    out.pop_back(); // remove trailing dot

    return out;
}

} // namespace