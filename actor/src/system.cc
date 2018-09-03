/**
    Copyright 2018 TryExceptElse

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#include "system.h"

#include <utility>
#include "uuid.h"

namespace kin {


System::System(std::unique_ptr<Body> root):
        System(GetUUID4(), std::move(root)) {}

System::System(std::string id, std::unique_ptr<Body> root):
        root_(std::move(root)) {
    id_ = id;
    universe_ = nullptr;
}

/**
 * Gets primary influence on passed point at passed time.
 * Position is relative to System origin, and time is relative to
 * System t0.
 */
const Body& System::FindPrimaryInfluence(const Vector r, const double t) const {
    // This method assumes that all bodies in the system are in a valid
    // state. In particular, it is assumed that children of the same
    // parent do not have overlapping spheres of influence.
    const Body *primary = root_.get();
    while (1) {
        for (const BodyIdPair &child_pair : primary->children()) {
            const Body &child = *child_pair.second;
            // check if position is within child sphere of influence
            double sphere_radius = child.sphere_of_influence();
            Vector child_r = child.PredictSystemPosition(t);
            double distance_sq = (child_r - r).squaredNorm();
            // if position is within radius of child sphere of
            // influence, repeat process, but now with child as root.
            if (distance_sq < sphere_radius * sphere_radius) {
                primary = &child;
                continue;
            }
        }
        break;  // No child soi contained passed position.
    }
    return *primary;
}

bool System::AddActor(Actor *actor) {
    // TODO
    // Check that actor is in universe
    // If actor already has a system, clear actor's system ref +
    // clear system's stored actor ID
    // Add actor to system.

    // Returns pair of iterator, bool
    const std::size_t initial_size = actor_ids.size();
    auto result = actor_ids.insert(actor->id());
    if (result.second) {
        throw std::runtime_error("System::AddActor : "
            "ID Already present in system");
    }
    return actor_ids.size() == initial_size;
}


}  // namespace kin

extern "C" kin::System* kin_NewSystemFromRoot(kin::Body *root) {
    return new kin::System(std::move(std::unique_ptr<kin::Body>(root)));
}

extern "C" kin::System* kin_NewSystemFromIdAndRoot(
        char *id, kin::Body *root) {
    return new kin::System(id, std::move(std::unique_ptr<kin::Body>(root)));
}
