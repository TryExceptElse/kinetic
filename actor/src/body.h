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

#include "./const.h"

namespace kin {


class Body {
 protected:
    const double GM_;
    const double r_;

 public:
    Body(const double GM, const double r): GM_(GM), r_(r) {}

    double mass() const { return GM_ / G; }
    double gm() const { return GM_; }
    double radius() const { return r_; }
};


}  // namespace kin

#endif  // ACTOR_SRC_BODY_H_
