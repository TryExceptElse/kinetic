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
