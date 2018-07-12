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

#ifndef ACTOR_SRC_UNIVERSE_H_
#define ACTOR_SRC_UNIVERSE_H_

#include <string>
#include <memory>

#include "system.h"
#include "actor.h"

namespace kin {


class Universe {
 public:
    // General methods
    bool AddSystem(std::unique_ptr<System> system);
    bool AddActor(std::unique_ptr<Actor> actor);

    SystemMap::iterator FindSystem(const std::string id) {
        return systems_.find(id);
    }

    ActorMap::iterator FindActor(const std::string id) {
        return actors_.find(id);
    }

    // Getters
    const SystemMap& systems() const { return systems_; }
    const ActorMap& actors() const { return actors_; }

 private:
    SystemMap systems_;
    ActorMap actors_;
};


}  // namespace kin

#endif  // ACTOR_SRC_UNIVERSE_H_
