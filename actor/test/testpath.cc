#include <memory>
#include <utility>
#include "catch.hpp"
#include "system.h"
#include "body.h"
#include "vector.h"
#include "orbit.h"
#include "path.h"


//         test name                  test group
TEST_CASE( "test path with no maneuvers can be calculated", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::FlightPath path(system, r, v, 0);
    // predict orbit of 1/2 period from t0.
    const kin::Orbit prediction = path.Predict(374942509.78053558 / 2);

    const kin::Vector position = prediction.position();
    REQUIRE( position.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( position.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( position.z == Approx(14442203602.998617).epsilon(0.0001) );
}