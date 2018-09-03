#include "catch.hpp"

#include <unordered_set>
#include <string>
#include "uuid.h"

//         test name                            test group
TEST_CASE( "uuid generates distributed values", "[UUID]" ) {
    constexpr long kNGenerations = 100000;
    std::unordered_set<std::string> uuid_set;
    for (long i = 0; i < kNGenerations; ++i) {
        uuid_set.insert(kin::GenerateSimpleUUID());
    }

    REQUIRE( uuid_set.size() == kNGenerations );
}

//         test name                            test group
TEST_CASE( "sole generates distributed values", "[UUID]" ) {
    constexpr long kNGenerations = 100000;
    std::unordered_set<std::string> uuid_set;
    for (long i = 0; i < kNGenerations; ++i) {
        uuid_set.insert(kin::GetUUID4());
    }

    REQUIRE( uuid_set.size() == kNGenerations );
}
