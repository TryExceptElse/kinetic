#include "catch.hpp"
#include "orbit.h"
#include "const.h"


//         test name               test group
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

TEST_CASE( "Body sphere of influence handles no parent case", "[Body]" ) {
    kin::Body body(kin::G * 10.0, 10.0);
    REQUIRE( body.sphere_of_influence() == -1.0 );
}

TEST_CASE( "Body child can be added", "[Body]" ) {
    kin::Body parent_body("1", kin::G * 100.0, 100.0);
    std::unique_ptr<kin::Body> child_body_ptr =
        std::make_unique<kin::Body>("2", kin::G * 10.0, 10.0);
    kin::Body &child_body = *child_body_ptr;
    parent_body.AddChild(std::move(child_body_ptr));

    REQUIRE( parent_body.IsParent(child_body) );
}

TEST_CASE( "Body child has pointer to parent stored", "[Body]" ) {
    kin::Body parent_body("1", kin::G * 100.0, 100.0);
    std::unique_ptr<kin::Body> child_body_ptr =
        std::make_unique<kin::Body>("2", kin::G * 10.0, 10.0);
    kin::Body &child_body = *child_body_ptr;
    parent_body.AddChild(std::move(child_body_ptr));

    REQUIRE( child_body.parent()->id() == parent_body.id() );
}
