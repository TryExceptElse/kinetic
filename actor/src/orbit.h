/**
 * Derived from work described at
 * http://quantumg.blogspot.com/2010/07/coding-orbital-mechanics.html
 */

// Glossary:
// A number of variable names are used internally within this module
// that are, or are adapted from commonly used annotations in physics.
// These variable names are only intended to be used internally, such
// as within methods or protected class fields. Public access occurs
// using a more explicit name.
// (For example: 'semi_major_axis()' accesses 'a')
//
// These names are only used within this file to represent the type of
// value listed here, unless it is obvious from context what their
// meaning is (for example: i as an index in an iterator).
//
// r : position vector
// v : velocity vector
// a : semi-major-axis
// e : eccentricity
// i : inclination
// l : longitude of ascending node
// w : argument of periapsis
// t : true anomaly
// u : standard gravitational parameter

#ifndef ACTOR_SRC_ORBIT_H_
#define ACTOR_SRC_ORBIT_H_

#include <memory>
#include "body.h"
#include "vector.h"

namespace kin {

// forward declarations
class Body;


// --------------------------------------------------------------------


class Orbit {
 public:
    Orbit(const Body &ref,
        double a, double e, double i, double l, double w, double t);

    Orbit(double u, double a, double e, double i, double l, double w, double t):
        u(u), a(a), e(e), i(i), l(l), w(w), t(t) {}

    Orbit(const Body &ref, const Vector r, const Vector v);

    Orbit(double u, const Vector r, const Vector v): u(u) {
        CalcFromPosVel(r, v);
    }

    // getters

    double gravitational_parameter() const { return u; }
    double semi_major_axis() const { return a; }
    double periapsis() const { return a * (1.0 - e); }
    double apoapsis() const { return e < 1 ? a * (1.0 + e) : -1.0; }
    double eccentricity() const { return e; }
    double inclination() const { return i; }
    double longitude_of_ascending_node() const { return l; }
    double argument_of_periapsis() const { return w; }
    double true_anomaly() const { return t; }
    double semiparameter() const { return a * (1 - e * e); }
    double semi_minor_axis() const;
    double period() const;
    double eccentric_anomaly() const;
    double mean_anomaly() const;
    double mean_motion() const;
    double time_since_periapsis() const;
    double min_speed() const;
    double max_speed() const;
    Vector position() const;
    Vector velocity() const;

    // vectors in geocentric equatorial inertial coordinates
    void CalcFromPosVel(const Vector r, const Vector v);

    // For small eccentricities a good approximation of true anomaly can be
    // obtained by the following formula (the error is of the order e^3)
    double EstimateTrueAnomaly(const double meanAnomaly) const;
    double CalcEccentricAnomaly(const double meanAnomaly) const;
    double SpeedAtDistance(const double distance) const;
    void CalcTrueAnomaly(const double eccentricAnomaly);
    void Step(const double time);
    Orbit Predict(const double time) const;

 protected:
    double u, a, e, i, l, w, t;
    Vector epoch_;
};


}  // namespace kin

#endif  // ACTOR_SRC_ORBIT_H_
