#include <random>
#include <cmath>

#ifndef __EMSCRIPTEN__
#include "sole.hpp"
#endif  // __EMSCRIPTEN__

namespace kin {


// If emscripten is not being used, sole is the preferred uuid lib
// Otherwise, use a custom uuid function to avoid the issues that arise
// from attempting to use sole.hpp in emscripten.

static const char *kUUIDChars =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static std::default_random_engine generator;
static std::uniform_int_distribution<int> distribution(0, sizeof(kUUIDChars));

std::string GenerateSimpleID() {
    std::string uuid = std::string(36, '-');
    uuid[14] = '4';
    for (int i = 0; i < 36; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 14 || i == 23) {
            continue;
        }
        uuid[i] = kUUIDChars[distribution(generator)];
    }
    return uuid;
}

#ifndef __EMSCRIPTEN__
std::string GetUUID4() { return sole::uuid4().str(); }
#else  // __EMSCRIPTEN__
// Not actually an UUID4, but close enough for what it is used for.
const auto GetUUID4 = GenerateSimpleID;
#endif  // __EMSCRIPTEN__


}  // namespace kin
