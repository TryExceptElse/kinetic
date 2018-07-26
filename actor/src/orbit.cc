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
    if (e >= 1) {
        throw std::runtime_error(
            "Orbit::period() : No period for orbits with e >= 1");
    }
    return 2 * PI * std::sqrt(a*a*a / u);
}

double Orbit::mean_motion() const {
    if (e >= 1.0) {
        throw std::runtime_error(
            "Orbit::mean_motion() : Eccentricity >= 1: " + std::string(e))
    }
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
    double E;
    if (0.0 < e && e < 1.0) {
        E = acos((e + cos(t)) / (1 + e * cos(t)));
    } else if (e > 1.0) {
        E = std::acosh((e + std::cos(t)) / (1.0 + e * std::cos(t)));
    } else if (e == 1.0) {
        E = std::tan(t / 2.0);
    } else if (e == 0.0) {
        E = t;
    } else {
        throw std::runtime_error("Orbit::eccentric_anomaly() : "
            "Invalid e: " + std::to_string(e));
    }
    if (t > PI && E < PI) {
        E = 2*PI - E;
    }
    return E;
}

double Orbit::mean_anomaly() const {
    // TODO: Should work regardless of eccentricity
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
    double p, q;
    // Generate positions in orbital plane.
    if (0.0 < e && e < 1.0) {
        p = a * (std::cos(ea) - e);
        q = a * std::sin(ea) * std::sqrt(1 - std::pow(e, 2));
    } else {
        double r;
        if (e > 1.0) {
            const double semi_latus = a * (1.0 - e * e);
            r = semi_latus / (1.0 + e * cos(t));
        } else if (e == 1.0) {
            // TODO
        } else if (e == 0.0) {
            r = a;
        } else {
            throw std::runtime_error("Orbit::position() : "
                "Invalid e: " + std::to_string(e));
        }
        p = std::cos(t) * r;
        q = std::sin(t) * r;
    }
    Vector untransformed_position(p, q, 0);

    // Transform position
    if (!transforms_initialized_) {
        CalculateTransform(untransformed_position);
    }
    const Vector in_plane_r = plane_transform_ * untransformed_position;
    return periapsis_transform_ * in_plane_r;
}

/**
 * Gets Current velocity vector in orbit.
 */
Vector Orbit::velocity() const {
    const double ea = eccentric_anomaly();
    // Transform velocity into 3d space.
    const double distance = a * (1 - e * std::cos(ea));
    const Vector untransformed_v = Vector(
            - std::sin(ea),  // vp
            std::sqrt(1 - e * e) * std::cos(ea),  // vq
            0
    ) * std::sqrt(u * a) / distance;
    // Transform velocity
    if (!transforms_initialized_) {
        CalculateTransform();
    }
    const Vector in_plane_v = plane_transform_ * untransformed_v;
    return periapsis_transform_ * in_plane_v;
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
    if (std::isnan(eccentric_anomaly)) {
        throw std::invalid_argument("Orbit::CalcTrueAnomaly passed NaN.");
    }
    const double E = eccentric_anomaly;
    this->t = std::acos((std::cos(E) - e) / (1 - e * std::cos(E)));
    if (eccentric_anomaly > PI && this->t < PI) {
        this->t = 2*PI - this->t;
    }
}

double Orbit::EstimateTrueAnomaly(const double mean_anomaly) const {
    if (std::isnan(mean_anomaly)) {
        throw std::invalid_argument("Orbit::EstimateTrueAnomaly passed NaN.");
    }
    const double M = mean_anomaly;
    return M + 2 * e * std::sin(M) + 1.25 * e * e * std::sin(2 * M);
}

double Orbit::CalcEccentricAnomaly(const double mean_anomaly) const {
    if (std::isnan(mean_anomaly)) {
        throw std::invalid_argument("Orbit::CalcEccentricAnomaly passed NaN.");
    }
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

void Orbit::CalculateTransform() const {
    // Roundabout + inefficient but simple way to ensure things are
    // initialized.
    position();
}

double Orbit::SpeedAtDistance(const double distance) const {
    return std::sqrt(u * (2 / distance - 1 / a));
}

void Orbit::Step(const double time) {
    if (!transforms_initialized_) {
        position();  // Initialize values before moving if they are not already.
    }
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
    if (!transforms_initialized_) {
        position();  // Cache values that may be copied
        // Since a single orbit may be copied many times, it
        // is best to calculate anything that can be cached once,
        // and then give that data to all copies, than to have each
        // copy calculate things for themselves.
    }
    // Create copy of self and advance.
    // Copy elision optimization should occur.
    Orbit prediction = *this;
    prediction.Step(time);
    prediction.plane_transform_ = plane_transform_;
    prediction.periapsis_transform_ = periapsis_transform_;
    return prediction;
}


}  // namespace kin
