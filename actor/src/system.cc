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

namespace kin {


System::System(std::unique_ptr<Body> root): root_(std::move(root)) {}

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
        for (const BodyIdPair &child_pair : root_->children()) {
            const Body &child = *child_pair.second;
            // check if position is within child sphere of influence
            double sphere_radius = child.sphere_of_influence();
            Vector child_r = child.PredictSystemPosition(t);
            double distance_sq = (child_r - r).sqlen();
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


}  // namespace kin
