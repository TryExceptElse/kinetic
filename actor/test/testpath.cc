#include <memory>
#include <utility>
#include "catch.hpp"

#define private public   // horribly hacky way to access private members
#define protected public

#include "system.h"
#include "body.h"
#include "vector.h"
#include "orbit.h"
#include "path.h"


// FLIGHT PATH --------------------------------------------------------


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

    // Create Initial Orbit
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);
    const double half_orbit_t = 374942509.78053558 / 2;

    // Add maneuver
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    path.Add(kin::Maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2000.0,  // DV
            performance,
            150.0,  // m0
            half_orbit_t));  // t0

    // predict orbit of 1/2 period from t0.
    const kin::KinematicData prediction1 = path.Predict(half_orbit_t);

    // This test doesn't try to determine a precise position, just that
    // the calculation can complete, and results in a changed orbit.
    const kin::Vector position1 = prediction1.r;
    REQUIRE( position1.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( position1.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( position1.z == Approx(14442203602.998617).epsilon(0.0001) );

    // predict orbit of 3/2 period from t0.
    // Orbit should have changed notably
    const kin::KinematicData prediction2 =
            path.Predict(374942509.78053558 * 3.0 / 2.0);
    const kin::Vector position2 = prediction2.r;
    REQUIRE( position2.x != Approx(-712305324741.15112).epsilon(0.01) );
    REQUIRE( position2.y != Approx(365151451881.22858).epsilon(0.01) );
    REQUIRE( position2.z != Approx(14442203602.998617).epsilon(0.01) );
}


TEST_CASE( "test add method adds maneuver to path", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);

    // Create Initial Orbit
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);
    const double half_orbit_t = 374942509.78053558 / 2;

    // Add maneuver
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    const double dv = 2000.0;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            dv,  // DV
            performance,
            150.0,  // m0
            half_orbit_t);  // t0
    path.Add(maneuver);

    REQUIRE( path.maneuvers_.size() == 1 );
    REQUIRE( path.maneuvers_.begin()->second->t0() == half_orbit_t );
}


TEST_CASE( "test path calc continues incomplete groups", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::FlightPath path(system, r, v, 0);
    // predict orbit of 1/2 period from t0.
    const double t1 = 374942509.78053558 / 2;
    const double tf = t1 * 2;
    path.Calculate(t1);
    path.Calculate(tf);

    // There should be only a single group.
    REQUIRE( path.cache_.groups.size() == 1 );
}


TEST_CASE( "test GetSegment returns different seg after maneuver", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);

    // Create Initial Orbit
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);
    const double half_orbit_t = 374942509.78053558 / 2;

    // Add maneuver
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    const double dv = 2000.0;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            dv,  // DV
            performance,
            150.0,  // m0
            half_orbit_t);  // t0
    path.Add(maneuver);

    const double t0 = maneuver.t0();
    const double t1 = maneuver.t1() + 1;

    const kin::FlightPath::Segment &segment0 = path.GetSegment(t0);
    const kin::FlightPath::Segment &segment1 = path.GetSegment(t1);

    REQUIRE(segment0.r0_ != segment1.r0_);
}


TEST_CASE( "test path maneuver increases velocity", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);

    // Create Initial Orbit
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);
    const double half_orbit_t = 374942509.78053558 / 2;

    // Add maneuver
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    const double dv = 2000.0;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            dv,  // DV
            performance,
            150.0,  // m0
            half_orbit_t);  // t0
    path.Add(maneuver);

    // predict orbit of 1/2 period from t0.
    const kin::KinematicData prediction0 = path.Predict(half_orbit_t);
    // predict orbit at end of burn
    const kin::KinematicData prediction1 = path.Predict(maneuver.t1());

    // This test doesn't try to determine a precise velocity, just that
    // it has increased a roughly expected amount.
    const kin::Vector v0 = prediction0.v;
    const kin::Vector v1 = prediction1.v;

    // New velocity should be increased by burn DV.
    REQUIRE( v1.len() == Approx(v0.len() + dv).epsilon(0.01) );
}


// BALLISTIC SEGMENT --------------------------------------------------


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


TEST_CASE( "test calculate does not overrun group tf", "[BallisticSegment]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 1000000.0;
    const double half_orbit = 374942509.78053558 / 2;
    const double tf = half_orbit + t0;

    // Create BallisticSegmentGroup
    kin::FlightPath::BallisticSegmentGroup segment_group(system, r, v, t0, tf);
    const kin::FlightPath::CalculationStatus status =
            segment_group.Calculate(tf);
    REQUIRE( status.end_t == tf );
}


TEST_CASE( "test calc marks incomplete groups", "[BallisticSegment]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 1000000.0;
    const double half_orbit = 374942509.78053558 / 2;
    const double tf = half_orbit + t0;

    // Create BallisticSegmentGroup
    kin::FlightPath::BallisticSegmentGroup segment_group(system, r, v, t0);
    const kin::FlightPath::CalculationStatus status =
            segment_group.Calculate(tf);
    REQUIRE( status.incomplete_element == true );
}


// MANEUVER -----------------------------------------------------------


TEST_CASE( "test prograde thrust vector is calculated", "[Maneuver]" ) {
    const kin::Body ref(kin::G * 5.972e24, 6371000.0);
    const kin::Vector r(50.0, -50.0, 2);
    const kin::Vector v(-0.7, -0.7, 0.0);
    const double t = 0.0;
    const kin::PerformanceData performance(3500, 10000);
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde, 100.0, performance, 80, 0.0);

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
    const kin::Maneuver maneuver(
            kin::Maneuver::kRetrograde, 100.0, performance, 80, 0.0);

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
    const kin::Maneuver maneuver(
            kin::Maneuver::kRadial, 100.0, performance, 80, 0.0);

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
    const kin::Maneuver maneuver(
            kin::Maneuver::kAntiRadial, 100.0, performance, 80, 0.0);

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
    const kin::Maneuver maneuver(
            kin::Maneuver::kNormal, 100.0, performance, 80, 0.0);

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
    const kin::Maneuver maneuver(
            kin::Maneuver::kAntiNormal, 100.0, performance, 80, 0.0);

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

TEST_CASE( "test maneuver has correct final mass fraction", "[Maneuver]" ) {
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            1216.4,  // DV
            performance,
            150.0,  // m0
            30);  // t0
    REQUIRE( maneuver.mass_fraction() == Approx(0.333333).epsilon(0.0001) );
}

TEST_CASE( "test maneuver has correct final expended mass", "[Maneuver]" ) {
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            1216.4,  // DV
            performance,
            150.0,  // m0
            30);  // t0
    REQUIRE( maneuver.expended_mass() == Approx(50.0).epsilon(0.0001) );
}

TEST_CASE( "test maneuver has correct duration", "[Maneuver]" ) {
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            1216.4,  // DV
            performance,
            150.0,  // m0
            30);  // t0
    REQUIRE( maneuver.duration() == Approx(7.5).epsilon(0.0001) );
}

TEST_CASE( "test maneuver has correct t1", "[Maneuver]" ) {
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            1216.4,  // DV
            performance,
            150.0,  // m0
            30);  // t0
    REQUIRE( maneuver.t1() == Approx(37.5).epsilon(0.0001) );
}


// --------------------------------------------------------------------


TEST_CASE( "test performance has correct flow rate", "[PerformanceData]" ) {
    const kin::PerformanceData performance(3000.0, 20000.0);  // Ve, Thrust
    REQUIRE( performance.flow_rate() == Approx(6.66666666667).epsilon(0.0001) );

}
