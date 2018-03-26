#include "catch.hpp"
#include "orbit.h"
#include "body.h"
#include "vector.h"
#include "const.h"

//         test name                                    test group
TEST_CASE( "test eccentricity is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE( orbit.eccentricity() == Approx(0.049051434386).epsilon(0.0001) );
}

//         test name                                       test group
TEST_CASE( "test semi-major axis is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.semi_major_axis() ==
        Approx(778952537739.3).epsilon(0.0001)
    );
}

TEST_CASE( "test inclination is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.inclination() ==
        Approx(0.022779777375753599428).epsilon(0.0001)
    );
}

TEST_CASE( "test long. of ascending node is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.longitude_of_ascending_node() ==
        Approx(1.7539566784283953105).epsilon(0.0001)
    );
}

TEST_CASE( "test arg. of periapsis is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.argument_of_periapsis() ==
        Approx(4.7916072768395219228).epsilon(0.0001)
    );
}

TEST_CASE( "test mean anomaly is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.mean_anomaly() ==
        Approx(5.4840069130835997768).epsilon(0.0001)
    );
}

TEST_CASE( "test true anomaly is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.true_anomaly() ==
        Approx(5.4106199190548593947).epsilon(0.0001)
    );
}

TEST_CASE( "test apoapsis is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.apoapsis() ==
        Approx(817161277034.0616).epsilon(0.0001)
    );
}

TEST_CASE( "test periapsis is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.periapsis() ==
        Approx(740743798444.591).epsilon(0.0001)
    );
}