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

#ifndef ACTOR_SRC_ACTOR_H_
#define ACTOR_SRC_ACTOR_H_

#include <string>
#include "path.h"
#include "vector.h"

namespace kin {


// forward declare due to circular dependency
class Universe;
class System;


class Actor {
 public:
    Actor();
    Actor(const Vector r, const Vector v);
    explicit Actor(const std::string &json);
    Actor(Universe *universe, const Vector r, const Vector v);
    Actor(Universe *universe, const std::string &json);

    const std::string& id() const { return id_; }
    Vector r() const { return r_; }
    Vector v() const { return v_; }
    Vector local_position() const { return r_; }
    Vector local_velocity() const { return v_; }
    Vector world_position() const;
    Vector world_velocity() const;
    const FlightPath& path() const { return *path_; }
 private:
    std::string id_;
    std::string actor_type_;
    Vector r_;  // orbital position vector
    Vector v_;  // orbital velocity vector
    std::unique_ptr<FlightPath> path_;
    Universe *universe_;
};


// alias of map storing actors by their id's
using ActorMap = std::unordered_map<std::string, std::unique_ptr<Actor> >;


}  // namespace kin

#endif  // ACTOR_SRC_ACTOR_H_
