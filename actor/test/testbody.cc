#include "catch.hpp"
#include "body.h"
#include "const.h"


//         test name                  test group
TEST_CASE( "Body m is calculated", "[Body]" ) {
    kin::Body body(kin::G * 10.0, 10.0);
    REQUIRE( body.mass() == 10.0 );
}
