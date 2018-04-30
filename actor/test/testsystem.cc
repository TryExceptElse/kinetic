#include <memory>
#include <utility>
#include "catch.hpp"
#include "system.h"
#include "body.h"
#include "vector.h"
#include "orbit.h"
#include "path.h"


TEST_CASE( "test system root returns passed body", "[System]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const std::string body_id = body->id();
    const kin::System system(std::move(body));

    REQUIRE( system.root().id() == body_id );
}

TEST_CASE( "test system generates id if none passed", "[System]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));

    REQUIRE( system.id() != "" );
    REQUIRE( system.id().length() > 8 );
}

TEST_CASE( "test system stores id if one passed", "[System]" ) {
    const std::string system_id = "1234567890";
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(system_id, std::move(body));

    REQUIRE( system.id() == system_id );
}
