#include "actor.h"

#include "uuid.h"

namespace kin {



Actor::Actor(const std::string actor_type, const std::string id,
        const Vector r, const Vector v):
            Actor(nullptr, actor_type, id, r, v) {}

Actor::Actor(Universe *universe, const std::string actor_type,
        const std::string id, const Vector r, const Vector v):
            universe_(universe),
            id_(id.empty() ? GetUUID4() : id),
            actor_type_(actor_type) {
    // TODO: Initialize path
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
