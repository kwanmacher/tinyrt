// MIT License
//
// Copyright (c) 2020 Kaikai Wang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <tuple>

namespace tinyrt {
template <typename TFloat, typename TBool = TFloat>
class Vec3T {
  using vec3_t = Vec3T<TFloat, TBool>;

 public:
  Vec3T() : Vec3T(.0f, .0f, .0f) {}
  Vec3T(TFloat x, TFloat y, TFloat z) : v_{x, y, z} {}

  inline auto* operator-> () { return &v_; }
  inline const auto* operator-> () const { return &v_; }
  inline TFloat& operator[](int idx) { return coords_[idx]; }
  inline const TFloat& operator[](int idx) const { return coords_[idx]; }

  inline vec3_t operator-() const { return vec3_t(-v_.x, -v_.y, -v_.z); }

  inline vec3_t operator+(const vec3_t& other) const {
    return vec3_t(v_.x + other->x, v_.y + other->y, v_.z + other->z);
  }

  inline vec3_t operator-(const vec3_t& other) const {
    return vec3_t(v_.x - other->x, v_.y - other->y, v_.z - other->z);
  }

  inline vec3_t operator*(const vec3_t& other) const {
    return vec3_t(v_.x * other->x, v_.y * other->y, v_.z * other->z);
  }

  inline vec3_t operator*(const TFloat scalar) const {
    return vec3_t(v_.x * scalar, v_.y * scalar, v_.z * scalar);
  }

  inline vec3_t operator/(const TFloat scalar) const {
    return vec3_t(v_.x / scalar, v_.y / scalar, v_.z / scalar);
  }

  inline TBool operator>(const vec3_t& other) const {
    return v_.x > other->x && v_.y > other->y && v_.z > other->z;
  }

  inline TBool operator<(const vec3_t& other) const {
    return v_.x < other->x && v_.y < other->y && v_.z < other->z;
  }

  inline TBool operator>=(const vec3_t& other) const {
    return v_.x >= other->x && v_.y >= other->y && v_.z >= other->z;
  }

  inline TBool operator<=(const vec3_t& other) const {
    return v_.x <= other->x && v_.y <= other->y && v_.z <= other->z;
  }

  inline TBool operator==(const vec3_t other) const {
    return v_.x == other->x && v_.y == other->y && v_.z == other->z;
  }

  inline TFloat dot(const vec3_t& other) const {
    return (v_.x * other->x + v_.y * other->y + v_.z * other->z);
  }

  inline void operator*=(const TFloat scalar) {
    v_.x *= scalar;
    v_.y *= scalar;
    v_.z *= scalar;
  }

  inline void operator/=(const TFloat scalar) {
    v_.x /= scalar;
    v_.y /= scalar;
    v_.z /= scalar;
  }

  inline void operator+=(const vec3_t& other) {
    v_.x += other->x;
    v_.y += other->y;
    v_.z += other->z;
  }

  inline void operator-=(const vec3_t& other) {
    v_.x -= other->x;
    v_.y -= other->y;
    v_.z -= other->z;
  }

  inline vec3_t cross(const vec3_t& other) const {
    return vec3_t(v_.y * other->z - v_.z * other->y,
                  v_.z * other->x - v_.x * other->z,
                  v_.x * other->y - v_.y * other->x);
  }

  inline TFloat norm2() const {
    return v_.x * v_.x + v_.y * v_.y + v_.z * v_.z;
  }
  inline TFloat norm() const { return std::sqrt(norm2()); }

  inline vec3_t normalize() const& { return *this / this->norm(); }

  inline vec3_t reflect(const vec3_t& normal) const {
    return normal * 2 * this->dot(normal) - *this;
  }

  inline TBool zero() const {
    const TFloat kEpsilon = 1e-6f;
    return std::abs(v_.x) < kEpsilon && std::abs(v_.y) < kEpsilon &&
           std::abs(v_.z) < kEpsilon;
  }

  inline TBool small() const {
    const TFloat kEpsilon = 1e-3f;
    return std::abs(v_.x) < kEpsilon && std::abs(v_.y) < kEpsilon &&
           std::abs(v_.z) < kEpsilon;
  }

  inline vec3_t max(const vec3_t& other) {
    return vec3_t(std::max(v_.x, other->x), std::max(v_.y, other->y),
                  std::max(v_.z, other->z));
  }

  inline vec3_t min(const vec3_t& other) {
    return vec3_t(std::min(v_.x, other->x), std::min(v_.y, other->y),
                  std::min(v_.z, other->z));
  }

  inline vec3_t max(const TFloat other) {
    return vec3_t(std::max(v_.x, other), std::max(v_.y, other),
                  std::max(v_.z, other));
  }

  inline vec3_t min(const TFloat other) {
    return vec3_t(std::min(v_.x, other), std::min(v_.y, other),
                  std::min(v_.z, other));
  }

  inline std::tuple<vec3_t, vec3_t, vec3_t> basis() const {
    static const vec3_t x(1.f, 0.f, 0.f);
    static const vec3_t y(0.f, 1.f, 0.f);
    vec3_t z = this->normalize();
    auto i = z.cross(x);
    if (i.zero()) {
      i = z.cross(y);
    }
    i = i.normalize();
    return {i, z, i.cross(z)};
  }

  inline TBool same(const vec3_t& other) const {
    return (*this - other).zero();
  }
  inline TBool close(const vec3_t& other) const {
    return (*this - other).small();
  }

  friend std::ostream& operator<<(std::ostream& os, const vec3_t& vec) {
    os << "Vec3{x=" << std::fixed << std::setprecision(4) << vec->x
       << ",y=" << vec->y << ",z=" << vec->z << "}";
    return os;
  }

 private:
  union {
    TFloat coords_[3];
    struct {
      TFloat x;
      TFloat y;
      TFloat z;
    } v_;
  };
};

using Vec3 = Vec3T<float, bool>;
}  // namespace tinyrt
