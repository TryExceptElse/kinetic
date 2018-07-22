/**
 * Derived from work described at
 * http://quantumg.blogspot.com/2010/07/coding-orbital-mechanics.html
 */

#include "orbit.h"

#include <cmath>
#include "const.h"


namespace kin {

// Constructors -------------------------------------------------------

Orbit::Orbit(const Body &ref,
    double a, double e, double i, double l, double w, double t):
    u(ref.gm()), a(a), e(e), i(i), l(l), w(w), t(t),
    transforms_initialized_(false) {}

Orbit::Orbit(const Body &ref, const Vector r, const Vector v):
    Orbit(ref.gm(), r, v) {
}

Orbit::Orbit(double u, const Vector r, const Vector v): u(u), r0_(r), v0_(v) {
    // Validate inputs
    if (r.squaredNorm() == 0.0) {
        throw std::invalid_argument("Orbit initialized with r of [0,0,0]");
    }
    // Initialize orbital parameters
    CalcFromPosVel(r, v);
    transforms_initialized_ = false;
}

// Getters ------------------------------------------------------------

double Orbit::semi_minor_axis() const {
    return std::sqrt(a * a * (1 - e * e));
}

double Orbit::period() const {
    return 2 * PI * std::sqrt(a*a*a / u);
}

double Orbit::mean_motion() const {
    return std::sqrt(u / (a*a*a));
}

double Orbit::time_since_periapsis() const {
    return mean_anomaly() / mean_motion();
}

double Orbit::min_speed() const {
    if (e >= 1) {
        throw std::invalid_argument(
            "Orbit::min_speed() : No minimum speed for orbits with e >= 1");
    }
    return SpeedAtDistance(apoapsis());
}

double Orbit::max_speed() const {
    return SpeedAtDistance(periapsis());
}

double Orbit::eccentric_anomaly() const {
    double E = acos((e + cos(t)) / (1 + e * cos(t)));
    if (t > PI && E < PI) {
        E = 2*PI - E;
    }
    return E;
}

double Orbit::mean_anomaly() const {
    double E = eccentric_anomaly();
    double M = E - e * sin(E);
    if (E > PI && M < PI) {
        M = 2*PI - M;
    }
    return M;
}

/**
 * Gets Current position vector in orbit.
 */
Vector Orbit::position() const {
    const double ea = eccentric_anomaly();
    // Generate positions in orbital plane.
    const double p = a * (std::cos(ea) - e);
    const double q = a * std::sin(ea) * std::sqrt(1 - std::pow(e, 2));
    Vector untransformed_position(p, q, 0);

    // Transform position
    if (!transforms_initialized_) {
        CalculateTransform(untransformed_position);
    }
    const Vector in_plane_r = plane_transform_ * untransformed_position;
    return periapsis_transform_ * in_plane_r;
    //return untransformed_position;
}

/**
 * Gets Current velocity vector in orbit.
 */
Vector Orbit::velocity() const {
    const double p = semiparameter();
    const double g = -sqrt(u/p);
    return {
        g * (cos(l)          * (sin(w + t) + e * sin(w)) +  // X
             sin(l) * cos(i) * (cos(w + t) + e * cos(w))),
        g * (sin(l)          * (sin(w + t) + e * sin(w)) -  // Y
             cos(l) * cos(i) * (cos(w + t) + e * cos(w))),
        -g * (sin(i) * (cos(w + t) + e * cos(w))),          // Z
    };
}

// Other Methods ------------------------------------------------------

/**
 * Calculates orbital elements from passed orbital vectors.
 */
void Orbit::CalcFromPosVel(const Vector r, const Vector v) {
    const Vector h = r.cross(v);   // Calculate angular momentum.
    const Vector n(-h.y(), h.x(), 0);  // Calculate ascending node vector.

    // calculate eccentricity vector and scalar
    Vector e = v.cross(h) / u - r / r.norm();
    this->e = e.norm();

    // calculate specific orbital energy and semi-major axis
    const double E = v.squaredNorm() * 0.5 - u / r.norm();
    this->a = -u / (E * 2);

    // calculate inclination
    this->i = acos(h.z() / h.norm());

    // calculate longitude of ascending node
    if (this->i == 0.0)
        this->l = 0;
    else if (n.y() >= 0.0)
        this->l = acos(n.x() / n.norm());
    else
        this->l = 2 * PI - acos(n.x() / n.norm());

    // calculate argument of periapsis
    if (this->i == 0.0)
        this->w = acos(e.x() / e.norm());
    else if (e.z() >= 0.0)
        this->w = acos(n.dot(e) / (n.norm() * e.norm()));
    else
        this->w = 2 * PI - acos(n.dot(e) / (n.norm() * e.norm()));

    // calculate true anomaly
    if (r.dot(v) >= 0.0)
        this->t = acos(e.dot(r) / (e.norm() * r.norm()));
    else
        this->t = 2 * PI - acos(e.dot(r) / (e.norm() * r.norm()));
}

/**
 * Gets the angle between the direction of periapsis and the current
 * position of the body, as seen from the main focus of the ellipse
 * (the point around which the object orbits).
 *
 * https://en.wikipedia.org/wiki/True_anomaly
 */
void Orbit::CalcTrueAnomaly(const double eccentric_anomaly) {
    const double E = eccentric_anomaly;
    this->t = std::acos((std::cos(E) - e) / (1 - e * std::cos(E)));
    if (eccentric_anomaly > PI && this->t < PI) {
        this->t = 2*PI - this->t;
    }
}

double Orbit::EstimateTrueAnomaly(const double mean_anomaly) const {
    const double M = mean_anomaly;
    return M + 2 * e * std::sin(M) + 1.25 * e * e * std::sin(2 * M);
}

double Orbit::CalcEccentricAnomaly(const double mean_anomaly) const {
    const double t = EstimateTrueAnomaly(mean_anomaly);
    double E = acos((e + cos(t)) / (1 + e * cos(t)));
    double M = E - e * sin(E);

    // iterate to get M closer to mean_anomaly
    double rate = 0.01;
    bool lastDec = false;
    while (1) {
        if (std::fabs(M - mean_anomaly) < 0.0000000000001) {
            break;
        }
        if (M > mean_anomaly) {
            E -= rate;
            lastDec = true;
        } else {
            E += rate;
            if (lastDec) {
                rate *= 0.1;
            }
        }
        M = E - e * sin(E);
    }

    if (mean_anomaly > PI && E < PI) {
        E = 2 * PI - E;
    }

    return E;
}

void Orbit::CalculateTransform(const Vector untransformed_position) const {
    if (!r0_.isZero(0) && !v0_.isZero(0)) {
        const Vector orbit_plane_normal = r0_.cross(v0_).normalized();
        plane_transform_ = Quaternion().setFromTwoVectors(
                Vector(0.0, 0.0, 1.0), orbit_plane_normal);
        const Vector r2 = plane_transform_ * untransformed_position;
        periapsis_transform_ = Quaternion().setFromTwoVectors(r2, r0_);
    } else {
        throw std::runtime_error("Transform from elements not yet supported");
    }
    transforms_initialized_ = true;
}

double Orbit::SpeedAtDistance(const double distance) const {
    return std::sqrt(u * (2 / distance - 1 / a));
}

void Orbit::Step(const double time) {
    double M = mean_anomaly();
    M += mean_motion() * time;

    // apply corrections to mean anomaly
    M = std::fmod(M, TAU);
    if (M < 0) {
        M = TAU + M;
    }

    // calculate true anomaly
    const double E = CalcEccentricAnomaly(M);
    CalcTrueAnomaly(E);
}

Orbit Orbit::Predict(const double time) const {
    // Create copy of self and advance.
    // Copy elision optimization should occur.
    Orbit prediction = *this;
    prediction.Step(time);
    prediction.plane_transform_ = plane_transform_;
    prediction.periapsis_transform_ = periapsis_transform_;
    return prediction;
}


}  // namespace kin
