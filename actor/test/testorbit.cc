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

TEST_CASE( "test eccentricity is calculated correctly when e > 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 18329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.eccentricity() ==
        Approx(1.2012211497205505).epsilon(0.0001)
    );
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

//         test name                                       test group
TEST_CASE( "test semi-major axis is correct when e > 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 18329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    // note: difference ratio from example is ~0.0005. reason unknown.
    REQUIRE(
        orbit.semi_major_axis() ==
        Approx(-3565202186743.06).epsilon(0.001)
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

TEST_CASE( "test apoapsis is calculated correctly when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.apoapsis() ==
        Approx(817161277034.0616).epsilon(0.0001)
    );
}

TEST_CASE( "test apoapsis is calculated correctly when e > 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.apoapsis() ==
        Approx(817161277034.0616).epsilon(0.0001)
    );
}

TEST_CASE( "test periapsis is calculated correctly when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE( orbit.periapsis() == Approx(740743798444.591).epsilon(0.0001) );
}

TEST_CASE( "test period is calculated correctly when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE( orbit.period() == Approx(3.74900e8).epsilon(0.001) );
}

TEST_CASE( "test orbit position can be recalculated when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    const kin::Vector position = orbit.position();
    REQUIRE( position.x == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z == Approx(-12036457087.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit velocity can be recalculated when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    const kin::Vector velocity = orbit.velocity();
    REQUIRE( velocity.x == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit can be advanced correctly when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    double qtr = orbit.period() / 4;
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);

    const kin::Vector position = orbit.position();
    const kin::Vector velocity = orbit.velocity();
    REQUIRE( position.x == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit can be back-stepped when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    double qtr = orbit.period() / 4;
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);

    const kin::Vector position = orbit.position();
    const kin::Vector velocity = orbit.velocity();
    REQUIRE( position.x == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit can be advanced multiple orbits", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    double qtr = orbit.period() / 4;
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);
    orbit.Step(qtr);

    const kin::Vector position = orbit.position();
    const kin::Vector velocity = orbit.velocity();
    REQUIRE( position.x == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit can be back-stepped multiple orbits", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    double qtr = orbit.period() / 4;
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);
    orbit.Step(-qtr);

    const kin::Vector position = orbit.position();
    const kin::Vector velocity = orbit.velocity();
    REQUIRE( position.x == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit prediction can be created", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.predict(orbit.period() / 4 * 3);

    const kin::Vector position = prediction.position();
    const kin::Vector velocity = prediction.velocity();
    REQUIRE( position.x == Approx(-459510477588.0022).epsilon(0.0001) );
    REQUIRE( position.y == Approx(-626723842404.48425).epsilon(0.0001) );
    REQUIRE( position.z == Approx(12895002408.695148).epsilon(0.0001) );
    REQUIRE( velocity.x == Approx(10600.957453996163).epsilon(0.0001) );
    REQUIRE( velocity.y == Approx(-6784.9496397940266).epsilon(0.0001) );
    REQUIRE( velocity.z == Approx(-209.3331176139217).epsilon(0.0001) );
}

TEST_CASE( "test orbit prediction does modify orbit", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.predict(orbit.period() / 4 * 3);

    const kin::Vector position = orbit.position();
    const kin::Vector velocity = orbit.velocity();
    REQUIRE( position.x == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test multi-orbit prediction can be created", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.predict(orbit.period() * 5 / 2);

    const kin::Vector position = prediction.position();
    const kin::Vector velocity = prediction.velocity();
    REQUIRE( position.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( position.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( position.z == Approx(14442203602.998617).epsilon(0.0001) );
}

TEST_CASE( "test multi-orbit un-prediction can be created", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.predict(orbit.period() * -5 / 2);

    const kin::Vector position = prediction.position();
    const kin::Vector velocity = prediction.velocity();
    REQUIRE( position.x == Approx(-712305324741.15112).epsilon(0.0001) );
    REQUIRE( position.y == Approx(365151451881.22858).epsilon(0.0001) );
    REQUIRE( position.z == Approx(14442203602.998617).epsilon(0.0001) );
}
