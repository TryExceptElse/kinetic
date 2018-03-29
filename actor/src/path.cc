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

#include "path.h"

namespace kin {


FlightPath::FlightPath(
    const System &system, const Vector r, const Vector v, double t):
        system_(system), r0_(r), v0_(v), t0_(t) {}

Orbit FlightPath::Predict(const double time) const {
//    Body &primary_body()
//    if (maneuvers_.size() == 0) {
//        return
//    }
//    return
}


}  // namespace kin
