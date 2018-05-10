#include <memory>
#include <utility>
#include "catch.hpp"

#define private public   // horribly hacky way to access private members

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
    const kin::KinematicData prediction = path.Predict(374942509.78053558 / 2);

    const kin::Vector position = prediction.r;
    REQUIRE( position.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( position.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( position.z == Approx(14442203602.998617).epsilon(0.0001) );
}

TEST_CASE( "test path can be calculated with a maneuver", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::FlightPath path(system, r, v, 0);
    // predict orbit of 1/2 period from t0.
    const kin::KinematicData prediction = path.Predict(374942509.78053558 / 2);

    const kin::Vector position = prediction.r;
    REQUIRE( position.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( position.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( position.z == Approx(14442203602.998617).epsilon(0.0001) );
}

TEST_CASE( "test segment can predict half orbit", "[BallisticSegment]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 100000.0;
    const double half_orbit = 374942509.78053558 / 2;
    const kin::FlightPath::BallisticSegment segment(system, r, v, t0);

    const kin::KinematicData result = segment.Predict(half_orbit + t0);

    REQUIRE( result.r.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( result.r.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( result.r.z == Approx(14442203602.998617).epsilon(0.0001) );
}

TEST_CASE( "test segment can calculate half orbit", "[BallisticSegment]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 100000.0;
    const double half_orbit = 374942509.78053558 / 2;
    const kin::FlightPath::BallisticSegment segment(system, r, v, t0);

    const double tf = half_orbit + t0;
    kin::FlightPath::CalculationStatus status = segment.Calculate(tf);

    REQUIRE( status.end_t > tf );
}

TEST_CASE( "test segment group can predict half orbit", "[BallisticSegment]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 1000000.0;
    const double half_orbit = 374942509.78053558 / 2;
    kin::FlightPath::BallisticSegmentGroup segment_group(system, r, v, t0);

    const double tf = half_orbit + t0;
    segment_group.Calculate(tf);
    const kin::KinematicData result = segment_group.Predict(tf);

    REQUIRE( result.r.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( result.r.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( result.r.z == Approx(14442203602.998617).epsilon(0.0001) );
}

TEST_CASE( "test prograde thrust vector is calculated", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 2);
    const kin::Vector v(-0.7, -0.7, 0.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Maneuver maneuver(kin::kPrograde, 100.0, performance, 80, 0.0);

    const kin::Vector result = maneuver.FindThrustVector(ref, r, v, 0.0);

    const kin::Vector expected(-0.70710678118, -0.70710678118, 0);

    REQUIRE( result.x == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.y == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.z == Approx(0.0             ).epsilon(0.0001) );
}

TEST_CASE( "test retrograde thrust vector is calculated", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 2);
    const kin::Vector v(-0.7, -0.7, 0.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Maneuver maneuver(kin::kRetrograde, 100.0, performance, 80, 0.0);

    const kin::Vector result = maneuver.FindThrustVector(ref, r, v, 0.0);

    const kin::Vector expected(-0.70710678118, -0.70710678118, 0);

    REQUIRE( result.x == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.y == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.z == Approx(0.0             ).epsilon(0.0001) );
}

TEST_CASE( "test radial thrust vector is calculated", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 0.0);
    const kin::Vector v(-0.7, -0.7, 3.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Maneuver maneuver(kin::kRadial, 100.0, performance, 80, 0.0);

    const kin::Vector result = maneuver.FindThrustVector(ref, r, v, 0.0);

    const kin::Vector expected(-0.70710678118, -0.70710678118, 0);

    REQUIRE( result.x == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.y == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.z == Approx(0.0             ).epsilon(0.0001) );
}

TEST_CASE( "test anti-radial thrust vector is calculated", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 0.0);
    const kin::Vector v(-0.7, -0.7, 0.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Maneuver maneuver(kin::kAntiRadial, 100.0, performance, 80, 0.0);

    const kin::Vector result = maneuver.FindThrustVector(ref, r, v, 0.0);

    const kin::Vector expected(-0.70710678118, -0.70710678118, 0);

    REQUIRE( result.x == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.y == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.z == Approx(0.0             ).epsilon(0.0001) );
}

TEST_CASE( "test normal thrust vector is calculated", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 2.0);
    const kin::Vector v(-0.7, -0.7, 0.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Maneuver maneuver(kin::kNormal, 100.0, performance, 80, 0.0);

    const kin::Vector result = maneuver.FindThrustVector(ref, r, v, 0.0);

    const kin::Vector expected(-0.70710678118, -0.70710678118, 0);

    REQUIRE( result.x == Approx(0.019992    ).epsilon(0.0001) );
    REQUIRE( result.y == Approx(-0.019992   ).epsilon(0.0001) );
    REQUIRE( result.z == Approx(-0.9996     ).epsilon(0.0001) );
}

TEST_CASE( "test anti-normal thrust vector is calculated", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 2.0);
    const kin::Vector v(-0.7, -0.7, 0.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Maneuver maneuver(kin::kAntiNormal, 100.0, performance, 80, 0.0);

    const kin::Vector result = maneuver.FindThrustVector(ref, r, v, 0.0);

    const kin::Vector expected(-0.70710678118, -0.70710678118, 0);

    REQUIRE( result.x == Approx(-0.019992   ).epsilon(0.0001) );
    REQUIRE( result.y == Approx(0.019992    ).epsilon(0.0001) );
    REQUIRE( result.z == Approx(0.9996      ).epsilon(0.0001) );
}

TEST_CASE( "test fixed thrust vector is returned correctly", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 2.0);
    const kin::Vector v(-0.7, -0.7, 0.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Vector fixed_vector(1.0, 2.0, 3.0);
    const kin::Maneuver maneuver(fixed_vector, 100.0, performance, 80, 0.0);

    const kin::Vector result = maneuver.FindThrustVector(ref, r, v, 0.0);

    const kin::Vector expected(-0.70710678118, -0.70710678118, 0);

    REQUIRE( result.x == Approx(1.0   ).epsilon(0.0001) );
    REQUIRE( result.y == Approx(2.0   ).epsilon(0.0001) );
    REQUIRE( result.z == Approx(3.0   ).epsilon(0.0001) );
}
