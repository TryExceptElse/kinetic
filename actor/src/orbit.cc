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
    u(ref.gm()), a(a), e(e), i(i), l(l), w(w), t(t) {}

Orbit::Orbit(const Body &ref, const Vector r, const Vector v):
    Orbit(ref.gm(), r, v) {
}

Orbit::Orbit(double u, const Vector r, const Vector v): u(u) {
    // Validate inputs
    if (r.sqlen() == 0.0) {
        throw std::invalid_argument("Orbit initialized with r of [0,0,0]");
    }
    // Initialize orbital parameters
    CalcFromPosVel(r, v);
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
    const double p = semiparameter();
    Vector r;
    r.x = p * (cos(l) * cos(w + t) - sin(l) * cos(i) * sin(w + t));
    r.y = p * (sin(l) * cos(w + t) + cos(l) * cos(i) * sin(w + t));
    r.z = p * sin(i) * sin(w + t);
    return r - epoch_;
}

/**
 * Gets Current velocity vector in orbit.
 */
Vector Orbit::velocity() const {
    const double p = semiparameter();
    Vector v;
    const double g = -sqrt(u/p);
    v.x = g * (cos(l)          * (sin(w + t) + e * sin(w)) +
               sin(l) * cos(i) * (cos(w + t) + e * cos(w)));
    v.y = g * (sin(l)          * (sin(w + t) + e * sin(w)) -
               cos(l) * cos(i) * (cos(w + t) + e * cos(w)));
    v.z = -g * (sin(i) * (cos(w + t) + e * cos(w)));
    return v;
}

// Other Methods ------------------------------------------------------

/**
 * Calculates orbital elements from passed orbital vectors.
 */
void Orbit::CalcFromPosVel(const Vector r, const Vector v) {
    const Vector h = r * v;        // calculate specific relative angular moment
    const Vector n(-h.y, h.x, 0);  // calculate vector to the ascending node

    // calculate eccentricity vector and scalar
    Vector e = ((v * h) * (1.0 / u)) - (r * (1.0 / r.len()));
    this->e = e.len();

    // calculate specific orbital energy and semi-major axis
    const double E = v.sqlen() * 0.5 - u / r.len();
    this->a = -u / (E * 2);

    // calculate inclination
    this->i = acos(h.z / h.len());

    // calculate longitude of ascending node
    if (this->i == 0.0)
        this->l = 0;
    else if (n.y >= 0.0)
        this->l = acos(n.x / n.len());
    else
        this->l = 2 * PI - acos(n.x / n.len());

    // calculate argument of periapsis
    if (this->i == 0.0)
        this->w = acos(e.x / e.len());
    else if (e.z >= 0.0)
        this->w = acos(n.Dot(e) / (n.len() * e.len()));
    else
        this->w = 2 * PI - acos(n.Dot(e) / (n.len() * e.len()));

    // calculate true anomaly
    if (r.Dot(v) >= 0.0)
        this->t = acos(e.Dot(r) / (e.len() * r.len()));
    else
        this->t = 2 * PI - acos(e.Dot(r) / (e.len() * r.len()));

    // calculate epoch
    this->epoch_ = Vector(0, 0, 0);
    this->epoch_ = position() - r;
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
    return prediction;
}


}  // namespace kin
