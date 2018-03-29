#include "actor.h"

namespace kin {

Actor::Actor(): Actor(Vector(0, 0, 0), Vector(0, 0, 0)) {}

Actor::Actor(const Vector r, const Vector v):
        r_(r), v_(v), universe_(nullptr) {}

Actor::Actor(const std::string &json): universe_(nullptr) {}

Actor::Actor(Universe *universe, const Vector r, const Vector v):
        r_(r), v_(v), universe_(universe) {}

Actor::Actor(Universe *universe, const std::string &json) {
    universe_ = universe;
    // todo: parse json
}


}  // namespace kin
