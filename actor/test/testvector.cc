#include "catch.hpp"
#include "vector.h"

//         test name                  test group
TEST_CASE( "len is calculated 1", "[Vector]" ) {
    kin::Vector vector(1.0, 1.0, 1.0);
    REQUIRE( vector.len() == Approx(1.73205080757).epsilon(0.0001) );
}

//         test name                  test group
TEST_CASE( "len is calculated 2", "[Vector]" ) {
    kin::Vector vector(2.0, 2.0, 2.0);
    REQUIRE( vector.len() == Approx(3.46410161514).epsilon(0.0001) );
}

//         test name                  test group
TEST_CASE( "square len is calculated 1", "[Vector]" ) {
    kin::Vector vector(1.0, 1.0, 1.0);
    REQUIRE( vector.sqlen() == 3 );
}

//         test name                  test group
TEST_CASE( "square len is calculated 2", "[Vector]" ) {
    kin::Vector vector(2.0, 2.0, 2.0);
    REQUIRE( vector.sqlen() == 12 );
}
