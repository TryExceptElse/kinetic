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

#ifndef ACTOR_SRC_BODY_H_
#define ACTOR_SRC_BODY_H_

#include <memory>
#include <string>
#include <unordered_map>
#include "const.h"
#include "orbit.h"
#include "vector.h"

namespace kin {

// forward declarations
class Orbit;
class Body;

// alias map of bodies with their id's
using BodyMap = std::unordered_map<std::string, std::unique_ptr<Body> >;
using BodyIdPair = std::pair<const std::string, std::unique_ptr<Body> >;


class Body {
 protected:
    const std::string id_;
    std::unique_ptr<Orbit> orbit_;
    Body *parent_;
    BodyMap children_;
    const double GM_;
    const double r_;

 public:
    Body(const double GM, const double r,
        const std::string id = "",
        Body *parent = nullptr, Orbit *orbit = nullptr);

    // Alternate constructor taking a required name arg first.
    Body(const std::string id, const double GM, const double r,
        Body *parent = nullptr, Orbit *orbit = nullptr):
            Body(GM, r, id, parent, orbit) {}

    // methods

    bool HasParent() const { return parent_ != nullptr; }
    bool AddChild(std::unique_ptr<Body> body);
    bool IsParent(const Body &body);  // Checks if direct parent of passed body.
    Orbit Predict(const double t) const;
    Vector PredictLocalPosition(const double t) const;
    Vector PredictSystemPosition(const double t) const;
    Vector PredictLocalVelocity(const double t) const;
    Vector PredictSystemVelocity(const double t) const;

    // getters

    const std::string& id() const { return id_; }
    const Body* parent() const { return parent_; }
    const Orbit* orbit() const { return orbit_.get(); }
    double mass() const { return GM_ / G; }
    double gm() const { return GM_; }
    double radius() const { return r_; }
    /** Calculates radius of sphere of influence. Returns -1 if no parent */
    double sphere_of_influence() const;
    const BodyMap& children() const { return children_; }
};


}  // namespace kin

#endif  // ACTOR_SRC_BODY_H_
