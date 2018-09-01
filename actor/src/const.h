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
#ifndef ACTOR_SRC_CONST_H_
#define ACTOR_SRC_CONST_H_

namespace kin {

namespace constants {

constexpr double PI = 3.1415926535897932384626433832795028841971;
constexpr double TAU = PI * 2;
constexpr double G = 6.67300e-11;
constexpr double e = 2.71828182845904523536;

}  // namespace constants

// Elevate most commonly used and unambiguous constants to the
// kin namespace.
using constants::PI;
using constants::TAU;
using constants::G;

}  // namespace kin

#endif  // ACTOR_SRC_CONST_H_
