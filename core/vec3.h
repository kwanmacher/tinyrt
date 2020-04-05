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
#include <iostream>

namespace tinyrt {
class Vec3 {
 public:
  Vec3() : Vec3(.0f, .0f, .0f) {}
  Vec3(float x, float y, float z) : v_{x, y, z} {}

  inline auto* operator-> () { return &v_; }
  inline const auto* operator-> () const { return &v_; }
  inline float& operator[](int idx) { return coords_[idx]; }
  inline const float& operator[](int idx) const { return coords_[idx]; }

  inline Vec3 operator-() const { return Vec3(-v_.x, -v_.y, -v_.z); }

  inline Vec3 operator+(const Vec3& other) const {
    return Vec3(v_.x + other->x, v_.y + other->y, v_.z + other->z);
  }

  inline Vec3 operator-(const Vec3& other) const {
    return Vec3(v_.x - other->x, v_.y - other->y, v_.z - other->z);
  }

  inline Vec3 operator*(const Vec3& other) const {
    return Vec3(v_.x * other->x, v_.y * other->y, v_.z * other->z);
  }

  inline Vec3 operator*(const float scalar) const {
    return Vec3(v_.x * scalar, v_.y * scalar, v_.z * scalar);
  }

  inline Vec3 operator/(const float scalar) const {
    return Vec3(v_.x / scalar, v_.y / scalar, v_.z / scalar);
  }

  inline bool operator>(const Vec3& other) const {
    return v_.x > other->x && v_.y > other->y && v_.z > other->z;
  }

  inline bool operator<(const Vec3& other) const {
    return v_.x < other->x && v_.y < other->y && v_.z < other->z;
  }

  inline bool operator==(const Vec3 other) const {
    return v_.x == other->x && v_.y == other->y && v_.z == other->z;
  }

  inline float dot(const Vec3& other) const {
    return (v_.x * other->x + v_.y * other->y + v_.z * other->z);
  }

  inline void operator*=(const float scalar) {
    v_.x *= scalar;
    v_.y *= scalar;
    v_.z *= scalar;
  }

  inline void operator/=(const float scalar) {
    v_.x /= scalar;
    v_.y /= scalar;
    v_.z /= scalar;
  }

  inline void operator+=(const Vec3& other) {
    v_.x += other->x;
    v_.y += other->y;
    v_.z += other->z;
  }

  inline void operator-=(const Vec3& other) {
    v_.x -= other->x;
    v_.y -= other->y;
    v_.z -= other->z;
  }

  inline Vec3 cross(const Vec3& other) const {
    return Vec3(v_.y * other->z - v_.z * other->y,
                v_.z * other->x - v_.x * other->z,
                v_.x * other->y - v_.y * other->x);
  }

  inline float norm2() const { return v_.x * v_.x + v_.y * v_.y + v_.z * v_.z; }
  inline float norm() const { return std::sqrt(norm2()); }

  inline Vec3 normalize() const& { return *this / this->norm(); }

  inline Vec3 reflect(const Vec3& normal) const {
    return normal * 2 * this->dot(normal) - *this;
  }

  inline bool zero() const {
    const float kEpsilon = 1e-6f;
    return std::abs(v_.x) < kEpsilon && std::abs(v_.y) < kEpsilon &&
           std::abs(v_.z) < kEpsilon;
  }

  inline bool small() const {
    const float kEpsilon = 1e-3f;
    return std::abs(v_.x) < kEpsilon && std::abs(v_.y) < kEpsilon &&
           std::abs(v_.z) < kEpsilon;
  }

  inline Vec3 max(const Vec3& other) {
    return Vec3(std::max(v_.x, other->x), std::max(v_.y, other->y),
                std::max(v_.z, other->z));
  }

  inline Vec3 min(const Vec3& other) {
    return Vec3(std::min(v_.x, other->x), std::min(v_.y, other->y),
                std::min(v_.z, other->z));
  }

  inline Vec3 max(const float other) {
    return Vec3(std::max(v_.x, other), std::max(v_.y, other),
                std::max(v_.z, other));
  }

  inline Vec3 min(const float other) {
    return Vec3(std::min(v_.x, other), std::min(v_.y, other),
                std::min(v_.z, other));
  }

  inline std::tuple<Vec3, Vec3, Vec3> basis() const {
    static const Vec3 x(1.f, 0.f, 0.f);
    static const Vec3 y(0.f, 1.f, 0.f);
    Vec3 z = this->normalize();
    auto i = z.cross(x);
    if (i.zero()) {
      i = z.cross(y);
    }
    i = i.normalize();
    return {i, z, i.cross(z)};
  }

  inline bool same(const Vec3& other) const { return (*this - other).zero(); }
  inline bool close(const Vec3& other) const { return (*this - other).small(); }

  friend std::ostream& operator<<(std::ostream& os, const Vec3& vec);

 private:
  union {
    float coords_[3];
    struct {
      float x;
      float y;
      float z;
    } v_;
  };
};
}  // namespace tinyrt
