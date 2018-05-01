#include "body.h"

#include <stdexcept>
#include <cmath>
#include "sole.hpp"

namespace kin {


Body::Body(const double GM, const double r): Body(sole::uuid4().str(), GM, r) {}

Body::Body(const double GM, const double r, std::unique_ptr<Orbit> orbit):
        Body(sole::uuid4().str(), GM, r) {}

Body::Body(const std::string id, const double GM, const double r):
        GM_(GM), r_(r), id_(id) {}

Body::Body(const std::string id, const double GM, const double r,
    std::unique_ptr<Orbit> orbit):
        Body(id, GM, r) {
    orbit_ = std::move(orbit);
}

bool Body::AddChild(std::unique_ptr<Body> child) {
    if (child == nullptr) {
        throw std::invalid_argument("Body::AddChild() : "
            "Passed body was null.");
    }
    if (children_.find(child->id()) != children_.end()) {
        // Nothing needs to be done.
        return false;
    }
    child->parent_ = this;
    children_[child->id()] = std::move(child);
    return true;
}

bool Body::IsParent(const Body &body) {
    return children_.find(body.id()) != children_.end();
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
