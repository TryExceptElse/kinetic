#include "catch.hpp"

#include <unordered_set>
#include <string>
#include "uuid.h"

//         test name                  test group
TEST_CASE( "uuid generates distributed values", "[UUID]" ) {
    constexpr int kNGenerations = 10000;
    std::unordered_set<std::string> uuid_set;
    for (int i = 0; i < kNGenerations; ++i) {
        uuid_set.insert(kin::GetUUID4());
    }

    REQUIRE( uuid_set.size() == kNGenerations );
}
