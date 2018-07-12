#include "actor.h"

#include "sole.hpp"

namespace kin {


Actor::Actor(): Actor(Vector(0, 0, 0), Vector(0, 0, 0)) {}

Actor::Actor(const Vector r, const Vector v):
        universe_(nullptr), id_(sole::uuid4().str()) {}

Actor::Actor(const std::string &json): universe_(nullptr) {}

Actor::Actor(Universe *universe, const Vector r, const Vector v):
        universe_(universe) {}

Actor::Actor(Universe *universe, const std::string &json) {
    universe_ = universe;
    // todo: parse json
}

KinematicData Actor::Predict(const double t) const {
    // If path is null, return zero'd kinematic data.
    if (path_.get() == nullptr) {
        return KinematicData();
    }
    // Otherwise check path.
    return path_->Predict(t);
}


}  // namespace kin
