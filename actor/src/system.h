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

#ifndef ACTOR_SRC_SYSTEM_H_
#define ACTOR_SRC_SYSTEM_H_

#include <set>
#include <memory>
#include "orbit.h"
#include "actor.h"
#include "body.h"

namespace kin {


class Universe;


class System {
 public:
    explicit System(std::unique_ptr<Body> root);
    System(std::string id, std::unique_ptr<Body> root);

    /**
     * Find body in System which is the primary influence at
     * passed position vector.
     */
    const Body& FindPrimaryInfluence(const Vector r, double t) const;
    void AddActor(Actor *actor);

    // getters
    Vector v() const { return v_; }
    const Body& root() const { return *root_; }
    const std::string id() const { return id_; }
 private:
    std::string id_;
    Universe *universe_;  // reference back to parent universe
    std::unique_ptr<Body> root_;  // root body object - others may have weak ptr
    Vector v_;  // system velocity relative to the avg of the stellar medium.
    std::set<std::string> actor_ids;  // allows lookup of actor in universe
};


// alias of map storing systems by their id's
using SystemMap = std::unordered_map<std::string, std::unique_ptr<System> >;


}  // namespace kin

#endif  // ACTOR_SRC_SYSTEM_H_
