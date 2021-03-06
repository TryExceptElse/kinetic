#include "body.h"

#include <stdexcept>
#include <cmath>
#include "uuid.h"

namespace kin {

Body::Body(const double GM, const double r, const std::string id,
        Body * const parent, Orbit * const orbit):
            GM_(GM), r_(r), id_(id.empty() ? GetUUID4() : id) {
    if (parent == nullptr) {
        parent_ = nullptr;
    } else {
        if (orbit == nullptr) {
            throw std::invalid_argument("Body::Body() : "
                "If parent body is passed, so must an orbit.");
        }
        parent_ = parent;
        orbit_ = std::make_unique<Orbit>(*orbit);
    }
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

KinematicData Body::PredictLocalKinematicData(const double t) const {
    return HasParent() ? Predict(t).kinematic_data() : KinematicData();
}

KinematicData Body::PredictSystemKinematicData(const double t) const {
    return PredictLocalKinematicData(t) + (
        HasParent() ? parent_->PredictSystemKinematicData(t) : KinematicData());
}

Vector Body::PredictLocalPosition(const double t) const {
    return HasParent() ? Predict(t).position() : Vector(0, 0, 0);
}

Vector Body::PredictSystemPosition(const double t) const {
    return PredictLocalPosition(t) + (
        HasParent() ? parent_->PredictSystemPosition(t) : Vector(0, 0, 0));
}

Vector Body::PredictLocalVelocity(const double t) const {
    return HasParent() ? Predict(t).velocity() : Vector(0, 0, 0);
}

Vector Body::PredictSystemVelocity(const double t) const {
    return PredictLocalVelocity(t) + (
        HasParent() ? parent_->PredictSystemVelocity(t) : Vector(0, 0, 0));
}


}  // namespace kin
