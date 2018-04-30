#include "catch.hpp"
#include "orbit.h"
#include "const.h"


//         test name                  test group
TEST_CASE( "Body m is calculated", "[Body]" ) {
    kin::Body body(kin::G * 10.0, 10.0);
    REQUIRE( body.mass() == 10.0 );
}

TEST_CASE( "Body id is generated if not passed", "[Body]" ) {
    kin::Body body(kin::G * 10.0, 10.0);
    REQUIRE( body.id() != "" );
    REQUIRE( body.id().length() > 8 );
}

TEST_CASE( "Body id is stored if passed", "[Body]" ) {
    kin::Body body("1234567890", kin::G * 10.0, 10.0);
    REQUIRE( body.id() == "1234567890" );
}
