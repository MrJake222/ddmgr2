#include "ipv6.hpp"

namespace dt {

std::string IPv6::ptr(int mask) const {
    std::string out;

    for (int i=7; i>=0; i--) {
        for (int j=3; j>=0; j--) {
            out += groups[i][j];
            out += ".";

            if ((i*4 + j)*4 == mask)
                goto end;
        }
    }

end:
    out.pop_back(); // remove trailing dot

    return out;
}

} // namespace