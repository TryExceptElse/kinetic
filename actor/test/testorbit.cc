#include "catch.hpp"

#include "vector.h"

#define private public   // horribly hacky way to access private members
#define protected public

#include "body.h"
#include "const.h"
#include "orbit.h"

//         test name                                    test group
TEST_CASE( "test eccentricity is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE( orbit.eccentricity() == Approx(0.049051434386).epsilon(0.001) );
}

TEST_CASE( "test eccentricity is calculated correctly at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE( orbit.eccentricity() ==
             Approx(0.1724889686910639).epsilon(0.001) );
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

TEST_CASE( "test semi-major axis is correct at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    // note: difference ratio from example is ~0.0005. reason unknown.
    REQUIRE(
        orbit.semi_major_axis() ==
        Approx(1029581412180.7297).epsilon(0.0001)
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

TEST_CASE( "test inclination is calculated correctly at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.inclination() ==
        Approx(0.4911867270769660343).epsilon(0.0001)
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

TEST_CASE( "test long. of ascending node is correct at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.longitude_of_ascending_node() ==
        Approx(1.0263958549477334437).epsilon(0.0001)
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

TEST_CASE( "test arg. of periapsis is correct at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.argument_of_periapsis() ==
        Approx(4.8245016671257214824).epsilon(0.0001)
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

TEST_CASE( "test mean anomaly is calculated correctly at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.mean_anomaly() ==
        Approx(6.16465186381).epsilon(0.0001)
        // 353.2088323063384 deg.
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

TEST_CASE( "test true anomaly is calculated correctly at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE(
        orbit.true_anomaly() ==
        Approx(6.1129309073297113386).epsilon(0.0001)
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

TEST_CASE( "test period is calculated correctly at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    REQUIRE( orbit.period() == Approx(569790352.385062).epsilon(0.001) );
}

TEST_CASE( "test eccentric anomaly is calculated correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    const double eccentric_anomaly = orbit.eccentric_anomaly();

    REQUIRE(
        eccentric_anomaly ==
        Approx(312.16343 / 180 * kin::PI).epsilon(0.0005)
    );
}

TEST_CASE( "test e anomaly is calculated correctly at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    const double eccentric_anomaly = orbit.eccentric_anomaly();

    REQUIRE(
        eccentric_anomaly ==
        Approx(6.1400461658150344846).epsilon(0.0005)
    );
}

TEST_CASE( "test calculated pos equals input pos when flat", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, 0.0);
    kin::Vector v(7320.0, 11329.0, 0.0);
    kin::Orbit orbit(body, r, v);
    REQUIRE( orbit.position().x() == Approx(617244712358.0).epsilon(0.00001) );
    REQUIRE( orbit.position().y() == Approx(-431694791368.0).epsilon(0.00001) );
    REQUIRE( orbit.position().z() == Approx(0.0).epsilon(0.00001) );
}

TEST_CASE( "test calculated position equals input position", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    REQUIRE( orbit.position().x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( orbit.position().y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( orbit.position().z() == Approx(-12036457087.0).epsilon(0.0001) );
}

TEST_CASE( "test calculated pos equals input pos at high i", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -402036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    REQUIRE( orbit.position().x() == Approx(617244712358.0).epsilon(0.001) );
    REQUIRE( orbit.position().y() == Approx(-431694791368.0).epsilon(0.001) );
    REQUIRE( orbit.position().z() == Approx(-402036457087.0).epsilon(0.001) );
}

TEST_CASE( "test orbit position can be recalculated when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    const kin::Vector position = orbit.position();
    REQUIRE( position.x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12036457087.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit velocity can be recalculated when e < 1", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);

    const kin::Vector velocity = orbit.velocity();
    REQUIRE( velocity.x() == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-0211.0).epsilon(0.0001) );
}

//TEST_CASE( "test position can be calculated from elements", "[Orbit]" ) {
//    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
//    kin::Orbit orbit(
//            body,
//            1029581412581.5106,
//            0.17248896869106262,
//            0.4911867270769660343,
//            1.0263958549477334437,
//            4.8245016671257214824,
//            5.4106199190548593947
//    );
//    REQUIRE( orbit.position().x() == Approx(617244712358.0).epsilon(0.001) );
//    REQUIRE( orbit.position().y() == Approx(-431694791368.0).epsilon(0.001) );
//    REQUIRE( orbit.position().z() == Approx(-402036457087.0).epsilon(0.001) );
//}

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
    REQUIRE( position.x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x() == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-0211.0).epsilon(0.0001) );
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
    REQUIRE( position.x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x() == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-0211.0).epsilon(0.0001) );
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
    REQUIRE( position.x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x() == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit can be back-stepped multiple orbits", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    double qtr = orbit.period() / 4;
    orbit.Step(-qtr);  // 0.25
    orbit.Step(-qtr);  // 0.5
    orbit.Step(-qtr);  // 0.75
    orbit.Step(-qtr);  // 1.
    orbit.Step(-qtr);  // 1.25
    orbit.Step(-qtr);  // 1.5
    orbit.Step(-qtr);  // 1.75
    orbit.Step(-qtr);  // 2
    orbit.Step(-qtr);  // 2.25
    orbit.Step(-qtr);  // 2.5
    orbit.Step(-qtr);  // 2.75
    orbit.Step(-qtr);  // 3

    const kin::Vector position = orbit.position();
    const kin::Vector velocity = orbit.velocity();
    REQUIRE( position.x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x() == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit prediction can be created", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.Predict(orbit.period() / 4 * 3);

    const kin::Vector position = prediction.position();
    const kin::Vector velocity = prediction.velocity();
    REQUIRE( position.x() == Approx(-457118078340.85107).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-665152002423.83655).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(13000874825.274738).epsilon(0.0001) );
    REQUIRE( velocity.x() == Approx(10600.957453996163).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(-6784.9496397940266).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-209.3331176139217).epsilon(0.0001) );
}

TEST_CASE( "test orbit prediction does modify orbit", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.Predict(orbit.period() / 4 * 3);

    const kin::Vector position = orbit.position();
    const kin::Vector velocity = orbit.velocity();
    REQUIRE( position.x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x() == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test multi-orbit prediction can be created", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.Predict(orbit.period() * 5 / 2);

    const kin::Vector position = prediction.position();
    REQUIRE( position.x() == Approx(-719081127257.40515).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(364854624247.81012).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(14595231066.511684).epsilon(0.0001) );
}

TEST_CASE( "test multi-orbit un-prediction can be created", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    const kin::Vector v(7320.0, 11329.0, -0211.0);
    const kin::Orbit orbit(body, r, v);
    const kin::Orbit prediction = orbit.Predict(orbit.period() * -5 / 2);

    const kin::Vector position = prediction.position();
    const kin::Vector velocity = prediction.velocity();
    REQUIRE( position.x() == Approx(-719081127257.40515).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(364854624247.81012).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(14595231066.511684).epsilon(0.0001) );
}

TEST_CASE( "test orbit can make a prediction 0s ahead", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit(body, r, v);
    kin::Orbit prediction = orbit.Predict(0.0);

    const kin::Vector position = prediction.position();
    const kin::Vector velocity = prediction.velocity();
    REQUIRE( position.x() == Approx(617244712358.0).epsilon(0.0001) );
    REQUIRE( position.y() == Approx(-431694791368.0).epsilon(0.0001) );
    REQUIRE( position.z() == Approx(-12036457087.0).epsilon(0.0001) );
    REQUIRE( velocity.x() == Approx(7320.0).epsilon(0.0001) );
    REQUIRE( velocity.y() == Approx(11329.0).epsilon(0.0001) );
    REQUIRE( velocity.z() == Approx(-0211.0).epsilon(0.0001) );
}

TEST_CASE( "test orbit half period prediction changes dir", "[Orbit]" ) {
    const kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    const kin::Vector r(617244712358.0,     -431694791368.0,    -12036457087.0);
    const kin::Vector v(7320.0,             11329.0,            -0211.0       );
    const double t0 = 100000.0;
    const double half_orbit = 374942509.78053558 / 2;
    const kin::Orbit orbit(body, r, v);

    const kin::Orbit result = orbit.Predict(half_orbit + t0);

    const kin::Vector dir1 = r.normalized();
    const kin::Vector dir2 = result.position().normalized();

    REQUIRE( dir1.x() != Approx(dir2.x()).epsilon(0.1) );
    REQUIRE( dir1.y() != Approx(dir2.y()).epsilon(0.1) );
    REQUIRE( dir1.z() != Approx(dir2.z()).epsilon(0.1) );
}

TEST_CASE( "test orbit can be copied correctly", "[Orbit]" ) {
    kin::Body body(kin::G * 1.98891691172467e30, 10.0);
    kin::Vector r(617244712358.0, -431694791368.0, -12036457087.0);
    kin::Vector v(7320.0, 11329.0, -0211.0);
    kin::Orbit orbit_a(body, r, v);
    kin::Orbit orbit_b = orbit_a;

    REQUIRE( orbit_a.u == orbit_b.u );
    REQUIRE( orbit_a.a == orbit_b.a );
    REQUIRE( orbit_a.t == orbit_b.t );
}
