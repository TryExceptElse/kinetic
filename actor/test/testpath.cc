#include <memory>
#include <utility>
#include <string>

#include "catch.hpp"

#include "vector.h"

#define private public   // horribly hacky way to access private members
#define protected public

#include "system.h"
#include "body.h"
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
    REQUIRE( position.x()== Approx(660798922159.6378173828).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-462156171007.35309).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12885777245.989922).epsilon(0.0001) );
}

/**
 * This tests for the effects of an improper update to the periapsis
 * rotation matrix.
 * This can manifest as all prediction positions existing in a line
 * passing through the orbited body and the initial position.
 */
TEST_CASE( "test path prediction changes r direction vector", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::FlightPath path(system, r, v, 0);
    // predict orbit of 1/2 period from t0.
    const kin::KinematicData prediction = path.Predict(374942509.78053558 / 2);

    const kin::Vector dir1 = r.normalized();
    const kin::Vector dir2 = prediction.r.normalized();

    REQUIRE( dir1.x() != Approx(dir2.x()).epsilon(0.1) );
    REQUIRE( dir1.y() != Approx(dir2.y()).epsilon(0.1) );
    REQUIRE( dir1.z() != Approx(dir2.z()).epsilon(0.1) );
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
    REQUIRE( position1.x()== Approx(660798922159.6378173828).epsilon(0.0001) );
    REQUIRE( position1.y() == Approx(-462156171007.35309).epsilon(0.0001) );
    REQUIRE( position1.z() == Approx(-12885777245.989922).epsilon(0.0001) );

    // predict orbit of 3/2 period from t0.
    // Orbit should have changed notably
    const kin::KinematicData prediction2 =
            path.Predict(374942509.78053558 * 3.0 / 2.0);
    const kin::Vector position2 = prediction2.r;
    REQUIRE( position2.x() != Approx(-712305324741.15112).epsilon(0.01) );
    REQUIRE( position2.y() != Approx(365151451881.22858).epsilon(0.01) );
    REQUIRE( position2.z() != Approx(14442203602.998617).epsilon(0.01) );
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
    REQUIRE( v1.norm() == Approx(v0.norm() + dv).epsilon(0.002) );
}


TEST_CASE( "test orbit prediction has correct primary influence", "[Path]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);

    // Create Initial Orbit
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);
    const double half_orbit_t = 374942509.78053558 / 2;

    // Make prediction
    const kin::OrbitData orbit_prediction = path.PredictOrbit(half_orbit_t);

    REQUIRE( orbit_prediction.body().id() == system.root().id() );
}

TEST_CASE( "Test odd edge case with repeated calculation succeeds", "[Path]") {

    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);

    const double period0 = 374942509.78053558;
    const kin::PerformanceData performance(3000, 20000);  // ve, thrust
    const double burn_start_t = period0 / 2;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2000,  // DV
            performance,
            150.0,  // m0
            period0 / 2);  // t0
    path.Add(maneuver);

    // generate first round of points
    for (int i = 0; i < 10; ++i) {
        double t = period0 / 10 * i;
        path.Predict(t);
    }

    // and then make an older prediction
    path.Predict(period0 / 2.0);
}

TEST_CASE( "Test r and v between segment groups match", "[Path]") {

    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);

    const double period0 = 374942509.78053558;
    const kin::PerformanceData performance(3000, 200);  // ve, thrust
    const double burn_start_t = period0 / 2;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2,  // DV
            performance,
            150.0,  // m0
            burn_start_t);  // t0
    path.Add(maneuver);
    
    const kin::FlightPath::Segment &seg0 = path.GetSegment(0.0);
    const kin::FlightPath::Segment &seg1 = path.GetSegment(burn_start_t);
    
    const kin::KinematicData seg0_end_data = seg0.Predict(burn_start_t);
    const kin::KinematicData seg1_start_data = seg1.Predict(burn_start_t);

    // Check that two different segments have been retrieved.
    REQUIRE( &seg0 != &seg1 );
    
    // Check that velocities match
    REQUIRE( seg0_end_data.v.x() == seg1_start_data.v.x() );
    REQUIRE( seg0_end_data.v.y() == seg1_start_data.v.y() );
    REQUIRE( seg0_end_data.v.z() == seg1_start_data.v.z() );
    
    // Check that positions match
    REQUIRE( seg0_end_data.r.x() == seg1_start_data.r.x() );
    REQUIRE( seg0_end_data.r.y() == seg1_start_data.r.y() );
    REQUIRE( seg0_end_data.r.z() == seg1_start_data.r.z() );
}

TEST_CASE( "Test r and v between segment groups match2", "[Path]") {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);

    const double period0 = 374942509.78053558;
    const kin::PerformanceData performance(3000, 20);  // ve, thrust
    const double burn_start_t = period0 / 2;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2,  // DV
            performance,
            150.0,  // m0
            burn_start_t);  // t0
    path.Add(maneuver);

    const double burn_end_t = maneuver.t1();

    const kin::FlightPath::Segment &seg0 = path.GetSegment(burn_start_t);
    const kin::FlightPath::Segment &seg1 = path.GetSegment(burn_end_t);

    const kin::KinematicData seg0_end_data = seg0.Predict(burn_end_t - 0.00001);
    const kin::KinematicData seg1_start_data = seg1.Predict(burn_end_t);

    // Check that two different segments have been retrieved.
    REQUIRE( &seg0 != &seg1 );

    // Check that velocities match
    REQUIRE( seg0_end_data.v.x() ==
             Approx(seg1_start_data.v.x()).epsilon(0.000001) );
    REQUIRE( seg0_end_data.v.y() ==
             Approx(seg1_start_data.v.y()).epsilon(0.000001) );
    REQUIRE( seg0_end_data.v.z() ==
             Approx(seg1_start_data.v.z()).epsilon(0.000001) );
}

TEST_CASE( "Test t between segments match up", "[Path]") {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, r, v, 0);

    const double period0 = 374942509.78053558;
    const kin::PerformanceData performance(3000, 20);  // ve, thrust
    const double burn_start_t = period0 / 2;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2,  // DV
            performance,
            150.0,  // m0
            burn_start_t);  // t0
    path.Add(maneuver);

    const double burn_end_t = maneuver.t1();

    const kin::FlightPath::Segment &seg0 = path.GetSegment(burn_start_t);
    const kin::FlightPath::Segment &seg1 = path.GetSegment(burn_end_t);

    REQUIRE( seg1.t0_ == burn_end_t );
}

TEST_CASE( "Test no abrupt velocity changes occur between segments", "[Path]") {

    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector start_r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector start_v(7320.0, 11329.0, -0211.0);
    kin::FlightPath path(system, start_r, start_v, 0);

    const double period0 = 374942509.78053558;
    const kin::PerformanceData performance(3000, 200);  // ve, thrust
    const double burn_start_t = period0 / 2;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2,  // DV
            performance,
            150.0,  // m0
            burn_start_t);  // t0
    path.Add(maneuver);

    const double test_start_t = maneuver.t0() - 3.0;
    const double test_end_t = maneuver.t1() + 3.0;
    const double test_duration = test_end_t - test_start_t;
    const int n_samples = 1000;
    const double delta_limit = 0.004;

    kin::Vector v, last_v, delta_v;
    double max_delta = 0.0;
    double max_delta_exceed_t;
    last_v = path.Predict(test_start_t).v;
    for (int i = 1; i < n_samples; ++i) {
        double t = test_duration / n_samples * i + test_start_t;
        v = path.Predict(t).v;
        delta_v = v - last_v;
        if (delta_v.norm() > max_delta) {
            max_delta = delta_v.norm();
            max_delta_exceed_t = t;
        }
        last_v = v;
    }
    REQUIRE( max_delta < delta_limit );
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

    REQUIRE( result.r.x() == Approx(660798922159.6378173828).epsilon(0.0001) );
    REQUIRE( result.r.y() == Approx(-462156171007.35309).epsilon(0.0001) );
    REQUIRE( result.r.z() == Approx(-12885777245.989922).epsilon(0.0001) );
}

/**
 * This tests for the effects of an improper update to the periapsis
 * rotation matrix.
 * This can manifest as all prediction positions existing in a line
 * passing through the orbited body and the initial position.
 */
TEST_CASE( "test half orbit prediction changes dir", "[BallisticSegment]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 100000.0;
    const double half_orbit = 374942509.78053558 / 2;
    const kin::FlightPath::BallisticSegment segment(system, r, v, t0);

    const kin::KinematicData result = segment.Predict(half_orbit + t0);

    const kin::Vector dir1 = r.normalized();
    const kin::Vector dir2 = result.r.normalized();

    REQUIRE( dir1.x() != Approx(dir2.x()).epsilon(0.1) );
    REQUIRE( dir1.y() != Approx(dir2.y()).epsilon(0.1) );
    REQUIRE( dir1.z() != Approx(dir2.z()).epsilon(0.1) );
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

TEST_CASE( "test ballistic seg calc + predict agree", "[BallisticSegment]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 1000000.0;
    const double half_orbit = 374942509.78053558 / 2;
    const double tf = half_orbit + t0;

    // Create BallisticSegment
    kin::FlightPath::BallisticSegment segment(system, r, v, t0);
    const kin::FlightPath::CalculationStatus status = segment.Calculate(tf);
    kin::KinematicData prediction = segment.Predict(status.end_t);

    // Check that prediction and calculation results are in agreement.
    REQUIRE( status.r.x() == prediction.r.x() );
    REQUIRE( status.r.y() == prediction.r.y() );
    REQUIRE( status.r.z() == prediction.r.z() );
}

// BALLISTIC SEGMENT GROUP --------------------------------------------

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

    REQUIRE( result.r.x() == Approx(660798922159.6378173828).epsilon(0.0001) );
    REQUIRE( result.r.y() == Approx(-462156171007.35309).epsilon(0.0001) );
    REQUIRE( result.r.z() == Approx(-12885777245.989922).epsilon(0.0001) );
}

/**
 * This tests for the effects of an improper update to the periapsis
 * rotation matrix.
 * This can manifest as all prediction positions existing in a line
 * passing through the orbited body and the initial position.
 */
TEST_CASE( "test group half orbit prediction vec", "[BallisticSegment]" ) {
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

    const kin::Vector dir1 = r.normalized();
    const kin::Vector dir2 = result.r.normalized();

    REQUIRE( dir1.x() != Approx(dir2.x()).epsilon(0.1) );
    REQUIRE( dir1.y() != Approx(dir2.y()).epsilon(0.1) );
    REQUIRE( dir1.z() != Approx(dir2.z()).epsilon(0.1) );
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

TEST_CASE( "test calc and prediction agree", "[BallisticSegmentGroup]" ) {
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
    kin::KinematicData prediction = segment_group.Predict(status.end_t);

    // Check that prediction and calculation results are in agreement.
    REQUIRE( status.r.x() == prediction.r.x() );
    REQUIRE( status.r.y() == prediction.r.y() );
    REQUIRE( status.r.z() == prediction.r.z() );
}

// MANEUVER SEGMENT GROUP ---------------------------------------------

TEST_CASE( "test calc status and prediction agree", "[ManeuverSegmentGroup]" ) {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::PerformanceData performance(3000, 200);  // ve, thrust
    const double burn_start_t = 0.0;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2,  // DV
            performance,
            150.0,  // m0
            burn_start_t);  // t0

    // Create SegmentGroup
    kin::FlightPath::ManeuverSegmentGroup segment_group(
        system, &maneuver, r, v, burn_start_t);
    const double tf = maneuver.t1();
    const kin::FlightPath::CalculationStatus status =
        segment_group.Calculate(tf);
    kin::KinematicData prediction = segment_group.Predict(tf - 0.000001);

    // Check that prediction and calculation results are in agreement.
    REQUIRE( status.r.x() == Approx(prediction.r.x()).epsilon(0.0000001) );
    REQUIRE( status.r.y() == Approx(prediction.r.y()).epsilon(0.0000001) );
    REQUIRE( status.r.z() == Approx(prediction.r.z()).epsilon(0.0000001) );
}

TEST_CASE( "test maneuver group does not overrun", "[ManeuverSegmentGroup]") {
    std::unique_ptr<kin::Body> body =
        std::make_unique<kin::Body>(kin::G * 1.98891691172467e30, 10.0);
    const kin::System system(std::move(body));
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::PerformanceData performance(3000, 200);  // ve, thrust
    const double burn_start_t = 0.0;
    const kin::Maneuver maneuver(
            kin::Maneuver::kPrograde,  // Maneuver Type
            2,  // DV
            performance,
            150.0,  // m0
            burn_start_t);  // t0

    // Create SegmentGroup
    kin::FlightPath::ManeuverSegmentGroup segment_group(
        system, &maneuver, r, v, burn_start_t);
    const double tf = maneuver.t1();
    const kin::FlightPath::CalculationStatus status =
        segment_group.Calculate(tf);

    REQUIRE( status.end_t == maneuver.t1() );
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

    REQUIRE( result.x() == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.y() == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.z() == Approx(0.0             ).epsilon(0.0001) );
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

    REQUIRE( result.x() == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.y() == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.z() == Approx(0.0             ).epsilon(0.0001) );
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

    REQUIRE( result.x() == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.y() == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.z() == Approx(0.0             ).epsilon(0.0001) );
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

    REQUIRE( result.x() == Approx(-0.70710678118  ).epsilon(0.0001) );
    REQUIRE( result.y() == Approx(0.70710678118   ).epsilon(0.0001) );
    REQUIRE( result.z() == Approx(0.0             ).epsilon(0.0001) );
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

    REQUIRE( result.x() == Approx(0.019992    ).epsilon(0.0001) );
    REQUIRE( result.y() == Approx(-0.019992   ).epsilon(0.0001) );
    REQUIRE( result.z() == Approx(-0.9996     ).epsilon(0.0001) );
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

    REQUIRE( result.x() == Approx(-0.019992   ).epsilon(0.0001) );
    REQUIRE( result.y() == Approx(0.019992    ).epsilon(0.0001) );
    REQUIRE( result.z() == Approx(0.9996      ).epsilon(0.0001) );
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

    REQUIRE( result.x() == Approx(1.0   ).epsilon(0.0001) );
    REQUIRE( result.y() == Approx(2.0   ).epsilon(0.0001) );
    REQUIRE( result.z() == Approx(3.0   ).epsilon(0.0001) );
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
