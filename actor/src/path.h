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
#include "body.h"
#include "vector.h"

namespace kin {


class Maneuver {
 public:
    Maneuver();
 private:
};

/**
 * A FlightPath is a series of Maneuvers and Trajectories, which taken
 * together allow the position and velocity at any time between t0 and
 * tf to be determined.
 */
class FlightPath {
 public:
    FlightPath();
    FlightPath(const Body &body, const Vector r, const Vector v);
 private:
    std::map<double, Maneuver> maneuvers_;
};


}  // namespace kin

#endif  // ACTOR_SRC_PATH_H_
