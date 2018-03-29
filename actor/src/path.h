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

#ifndef ACTOR_SRC_PATH_H_
#define ACTOR_SRC_PATH_H_

#include <map>
#include <memory>
#include "vector.h"
#include "orbit.h"

namespace kin {

class System;

/**
 * Enum defining the type of maneuver being performed.
 * This will determine the direction of acceleration over time.
 */
enum ManeuverType {
    kPrograde, kRetrograde, kNormal, kAntiNormal, kRadial, kAntiRadial, kFixed
};


class Maneuver {
 public:
    explicit Maneuver(ManeuverType type);
    Maneuver(ManeuverType type, const Body &ref,
        const Vector r0, const Vector p0);
 private:
    ManeuverType type_;  // type of maneuver
    Body &ref_;  // reference body
    Vector r0_;  // initial position relative to body
    Vector p0_;  // initial velocity relative to body
    double dv_;  // delta-V of maneuver
};

/**
 * A FlightPath is a series of Maneuvers and Trajectories, which taken
 * together allow the position and velocity at any time between t0 and
 * tf to be determined.
 */
class FlightPath {
 public:
    FlightPath(const System &system, const Vector r, const Vector v, double t);

    /** Gets Orbit object for passed point in time since t0 */
    Orbit Predict(const double time) const;

 private:
    std::map<double, Maneuver> maneuvers_;
    // raw pointer should never be invalid; system owns actor
    const System &system_;
    const Vector r0_;  // position relative to system origin
    const Vector v0_;  // velocity relative to system
    const double t0_;  // start time of flight path relative to system
};


}  // namespace kin

#endif  // ACTOR_SRC_PATH_H_
