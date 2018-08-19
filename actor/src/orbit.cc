/**
 * Derived loosely from work described at
 * http://quantumg.blogspot.com/2010/07/coding-orbital-mechanics.html
 */

#include "orbit.h"

#include <cmath>
#include "const.h"


namespace kin {

// Forward declarations
static double kepler(const double e, double mean_anomaly);


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
        return mean_anomaly() / time_since_periapsis();
    }
    return std::sqrt(u / (a*a*a));
}

double Orbit::time_since_periapsis() const {
    if (e > 1.0) {
        return std::sqrt(-a * -a * -a / u) * mean_anomaly();
    } else if (e == 1.0) {
        // TODO
        throw std::runtime_error("NOT IMPLEMENTED L61");
    }
    // If elliptical:
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
        if (t > PI) {
            E = -E;
        }
    } else if (e == 1.0) {
        E = std::tan(t / 2.0);
    } else if (e == 0.0) {
        E = t;
    } else {
        throw std::runtime_error("Orbit::eccentric_anomaly() : "
            "Invalid e: " + std::to_string(e));
    }
    if (e < 1.0 && t > PI && E < PI) {
        E = 2*PI - E;
    }
    return E;
}

double Orbit::mean_anomaly() const {
    double E = eccentric_anomaly();
    double M;
    if (0.0 < e && e < 1.0) {
        M = E - e * std::sin(E);
        if (E > PI && M < PI) {
            M = 2*PI - M;
        }
    } else if (e > 1.0) {
        M = e * std::sinh(E) - E;
    } else {
        throw std::runtime_error("NOT IMPLEMENTED L115");
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
            throw std::runtime_error("NOT IMPLEMENTED L124");
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
    Vector untransformed_v;
    if (0.0 < e && e < 1.0) {
        untransformed_v = std::sqrt(u * a) / distance * Vector(
            - std::sin(ea),  // vp
            std::sqrt(1 - e * e) * std::cos(ea),  // vq
            0);
    } else if (e > 1.0) {
        const double dist = a * (1 - e * e) / (1 + e * std::cos(t));
        const double phi = std::atan(e * std::sin(t) / (1 + e * std::cos(t)));
        const double angle = t + PI / 2 - phi;
        untransformed_v = std::sqrt(u * (2 / dist - 1 / a)) * Vector(
            std::cos(angle),
            std::sin(angle),
            0);
        // throw std::runtime_error("NOT IMPLEMENTED L171");
    } else {
        throw std::runtime_error("NOT IMPLEMENTED L172");
    }
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
    this->t = acos(e.dot(r) / (e.norm() * r.norm()));
    if (r.dot(v) < 0.0) {
        this->t = TAU - this->t;
    }
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
    if (0.0 < e && e < 1.0) {
        this->t = std::acos((std::cos(E) - e) / (1 - e * std::cos(E)));
        if (eccentric_anomaly > PI && this->t < PI) {
            this->t = 2*PI - this->t;
        }
    } else if (e > 1.0) {
        this->t =
            2 * std::atan(std::sqrt((e + 1) / (e - 1)) * std::tanh(E / 2));
        if (eccentric_anomaly < 0) {
            this->t = 2*PI + this->t;
        }
    } else {
        throw std::runtime_error("NOT IMPLEMENTED L230");
    }
}

double Orbit::EstimateTrueAnomaly(const double mean_anomaly) const {
    if (std::isnan(mean_anomaly)) {
        throw std::invalid_argument("Orbit::EstimateTrueAnomaly passed NaN.");
    }
    const double M = mean_anomaly;
    double t;
    if (0.0 < e && e < 1.0) {
        t = M + 2 * e * std::sin(M) + 1.25 * e * e * std::sin(2 * M);
    } else if (e > 1.0) {
        t = M + 2 * e * std::sinh(M) + 1.25 * e * e * std::sinh(2 * M);
    } else if (e == 1.0) {
    } else if (e == 0.0) {
    } else {
    }
    return t;
}

double Orbit::CalcEccentricAnomaly(const double mean_anomaly) const {
    // This method is used to determine the eccentric anomaly at a
    // point in time in the future, and (usually) not the current time.
    if (std::isnan(mean_anomaly)) {
        throw std::invalid_argument("Orbit::CalcEccentricAnomaly passed NaN.");
    }
    const double eccentric_anomaly = kepler(e, mean_anomaly);
    if (std::isnan(eccentric_anomaly)) {
        throw std::invalid_argument("Orbit::CalcEccentricAnomaly : "
            "has invalid values.");
    }
    return eccentric_anomaly;
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
    // position() calls CalculateTransform in its implementation.
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
    if (e < 1.0) {
        M = std::fmod(M, TAU);
        if (M < 0) {
            M = TAU + M;
        }
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


// FUNCTIONS FOR SOLVING KEPLER'S EQUATION


static constexpr double    kThresh         = 1e-12;
static constexpr double    kMinThresh      = 1e-14;
static constexpr int       kMaxIterations  = 7;
static constexpr double    kNearParabolicEccentricity = 1.01;


static double near_parabolic(const double E, const double e) {
    const double anomaly_b = e > 1. ? E * E : -E * E;
    double term = e * anomaly_b * E / 6.;
    double r_val = (1. - e) * E - term;
    int n = 4;

    while (std::fabs(term) > 1e-15) {
        term *= anomaly_b / (n * (n + 1));
        r_val -= term;
        n += 2;
    }
    return r_val;
}


static double kepler(const double e, double mean_anomaly) {
    double curr, err, thresh, offset = 0.;
    double delta_curr = 1.;
    bool is_negative = false;
    int n_iter = 0;

    if (!mean_anomaly) {
        return( 0.);
    }

    if (e < 1.0) {
        if (mean_anomaly < -PI || mean_anomaly > PI) {
            double tmod = std::fmod(mean_anomaly, PI * 2.);

            if (tmod > PI) {
                tmod -= 2. * PI;
            } else if (tmod < -PI) {
                tmod += 2. * PI;
            }
            offset = mean_anomaly - tmod;
            mean_anomaly = tmod;
        }

        if (e < .9) {  // Meeus' low-eccentricity method.
            curr = std::atan2(sin(mean_anomaly), cos(mean_anomaly) - e);
            // Make corrections.
            do {
                err = (curr - e * std::sin(curr) - mean_anomaly) /
                    (1. - e * std::cos(curr));
                curr -= err;
            } while (std::fabs(err) > kThresh);
            return curr + offset;
        }
    }

    if (mean_anomaly < 0.) {
        mean_anomaly = -mean_anomaly;
        is_negative = true;
    }

    curr = mean_anomaly;
    thresh = kThresh * std::fabs(1. - e);
    // Due to round-off error, there's no way to
    // go below a certain minimum threshold.
    if (thresh < kMinThresh) {
        thresh = kMinThresh;
    }
    if (e > 1. && mean_anomaly / e > 3.) {  // Hyperbolic, large-mean-anomaly
        curr = log(mean_anomaly / e) + 0.85;
    } else if ((e > .8 && mean_anomaly < PI / 3.) || e > 1.) {  // Up to 60d.
        double trial = mean_anomaly / std::fabs(1. - e);

        if (trial * trial > 6. * std::fabs(1. - e)) {
            trial = std::cbrt(6. * mean_anomaly);
        }
        curr = trial;
        if (thresh > kThresh) {  // Happens when e > 2.
            thresh = kThresh;
        }
    }

    if (e < 1.) {
        while (std::fabs(delta_curr) > thresh) {
            if (n_iter++ > kMaxIterations) {
                err = near_parabolic(curr, e) - mean_anomaly;
            } else {
                err = curr - e * std::sin(curr) - mean_anomaly;
            }
            delta_curr = -err / (1. - e * cos(curr));
            curr += delta_curr;
            if (n_iter > kMaxIterations * 2) {
                throw std::runtime_error("Runaway iteration");
            }
        }
    } else {
        while (std::fabs(delta_curr) > thresh) {
            if (n_iter++ > kMaxIterations && e < kNearParabolicEccentricity) {
                err = -near_parabolic(curr, e) - mean_anomaly;
            } else {
                err = e * std::sinh(curr) - curr - mean_anomaly;
            }
            delta_curr = -err / (e * std::cosh(curr) - 1.);
            curr += delta_curr;
            if (n_iter > kMaxIterations * 2) {
                throw std::runtime_error("Runaway iteration");
            }
        }
    }
    return is_negative ? offset - curr : offset + curr;
}


}  // namespace kin
