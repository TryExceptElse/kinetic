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

#ifndef ACTOR_SRC_VECTOR_H_
#define ACTOR_SRC_VECTOR_H_

#include <cmath>

namespace kin {


class Vector {
 public:
    double x, y, z;

    Vector() : x(0), y(0), z(0) { }
    Vector(double x, double y, double z) : x(x), y(y), z(z) { }

    double sqlen() const { return x * x + y * y + z * z; }
    double len() const { return std::sqrt(sqlen()); }

    double Dot(const Vector other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector Cross(const Vector other) const {
        return Vector(y * other.z - z * other.y,
                      z * other.x - x * other.z,
                      x * other.y - y * other.x);
    }

    Vector operator* (const Vector other) const { return Cross(other); }

    Vector operator* (const double scalar) const {
        return Vector(scalar * x, scalar * y, scalar * z);
    }

    Vector operator- (const double scalar) const {
        return Vector(x - scalar, y - scalar, z - scalar);
    }

    Vector operator+ (const Vector other) const {
        return Vector(x + other.x, y + other.y, z + other.z);
    }

    Vector operator- (const Vector other) const {
        return Vector(x - other.x, y - other.y, z - other.z);
    }

    bool operator== (const Vector other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};


}  // namespace kin

#endif  // ACTOR_SRC_VECTOR_H_
