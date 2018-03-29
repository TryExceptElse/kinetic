#include "body.h"

#include <stdexcept>
#include <cmath>

namespace kin {


Body::Body(const double GM, const double r): GM_(GM), r_(r) {}

Body::Body(const double GM, const double r, std::unique_ptr<Orbit> orbit):
        Body(GM, r) {
    orbit_ = std::move(orbit);
}

double Body::sphere_of_influence() const {
    // check that SOI can be calculated. If not, return -1.
    if (!HasParent()) {
        return -1.0;
    }
    return std::pow(orbit_->semi_major_axis() * gm() / parent_->gm(), 0.4);
}

Orbit Body::Predict(const double t) const { return orbit_->Predict(t); }

Vector Body::PredictLocalPosition(const double t) const {
    return HasParent() ? Predict(t).position() : Vector(0, 0, 0);
}

Vector Body::PredictSystemPosition(const double t) const {
    return PredictLocalPosition(t) + (
        HasParent() ? Vector(0, 0, 0) : parent_->PredictSystemPosition(t));
}

Vector Body::PredictLocalVelocity(const double t) const {
    return HasParent() ? Predict(t).velocity() : Vector(0, 0, 0);
}

Vector Body::PredictSystemVelocity(const double t) const {
    return PredictLocalVelocity(t) + (
        HasParent() ? Vector(0, 0, 0) : parent_->PredictSystemVelocity(t));
}


}  // namespace kin
