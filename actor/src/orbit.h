/**
 * Derived from work described at
 * http://quantumg.blogspot.com/2010/07/coding-orbital-mechanics.html
 */

#ifndef KINETIC_ORBIT_H_
#define KINETIC_ORBIT_H_

#include <memory>
#include "body.h"
#include "vector.h"

namespace kin {


// --------------------------------------------------------------------


class Orbit {

public:
    Orbit(const Body &ref,
        double a, double e, double i, double l, double w, double t):
        ref(ref), a(a), e(e), i(i), l(l), w(w), t(t) {}

    Orbit(const Body &ref, Vector r, Vector v): ref(ref) {
        CalcFromPosVel(r, v);
    }

    const Body& reference() const { return ref; }
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

    Vector Position() const;
    Vector Velocity() const;

    // vectors in geocentric equatorial inertial coordinates
    void CalcFromPosVel(const Vector r, const Vector v);

    // For small eccentricities a good approximation of true anomaly can be
    // obtained by the following formula (the error is of the order e^3)
    double EstimateTrueAnomaly(const double meanAnomaly) const;
    double CalcEccentricAnomaly(const double meanAnomaly) const;
    void CalcTrueAnomaly(const double eccentricAnomaly);
    void Step(const double time);

protected:
    double a, e, i, l, w, t;
    Vector epoch;
    const Body &ref;
};


} // namespace kin

#endif // include guard
