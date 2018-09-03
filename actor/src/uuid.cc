#ifndef __EMSCRIPTEN__
#include "sole.hpp"
#endif  // __EMSCRIPTEN__

namespace kin {


// If emscripten is not being used, sole is the preferred uuid lib
// Otherwise, use a custom uuid function to avoid the issues that arise
// from attempting to use sole.hpp in emscripten.

static constexpr char *kUUIDChars =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::string GenerateSimpleUUID() {
    std::string uuid = std::string(36, '-');
    int rnd = 0;
    int r = 0;

    uuid[14] = '4';

    for (int i = 0; i < 36; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 14 || i == 23) {
            continue;
        }
        if (rnd <= 0x02) {
            rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;
        }
        rnd >>= 4;
        uuid[i] = kUUIDChars[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
    }
    return uuid;
}

#ifndef __EMSCRIPTEN__
std::string GetUUID4() { return sole::uuid4().str(); }
#else  // __EMSCRIPTEN__
using GetUUID4 = GenerateSimpleUUID;
#endif  // __EMSCRIPTEN__


}  // namespace kin
