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

#include "core/intersect.h"

namespace tinyrt {
std::optional<Intersection> intersect(const Ray& ray,
                                      const Triangle& triangle) {
  const float EPSILON = 1e-6f;
  auto ab = triangle.b().coord - triangle.a().coord;
  auto ac = triangle.c().coord - triangle.a().coord;
  auto h = ray.direction.cross(ac);
  auto a = ab.dot(h);
  if (a > -EPSILON && a < EPSILON) {
    return std::nullopt;
  }
  auto f = 1.f / a;
  auto s = ray.origin - triangle.a().coord;
  auto u = f * s.dot(h);
  if (u < 0.0 || u > 1.0) {
    return std::nullopt;
  }
  auto q = s.cross(ab);
  auto v = f * ray.direction.dot(q);
  if (v < 0.0 || u + v > 1.0) {
    return std::nullopt;
  }
  float t = f * ac.dot(q);
  if (t <= EPSILON) {
    return std::nullopt;
  }
  return Intersection(ray, t, Vec3(u, v, 0.f), triangle, triangle.material());
}

std::optional<Intersection> intersect(const Ray& ray,
                                      const AVX2Triangle& triangles,
                                      const float tEntry, const float tExit) {
  static const AVX2Float EPSILON = 1e-6f;
  static const AVX2Float ZERO = 0.f;
  static const AVX2Float ONE = 1.f;
  AVX2Vec3 origin(ray.origin->x, ray.origin->y, ray.origin->z);
  AVX2Vec3 direction(ray.direction->x, ray.direction->y, ray.direction->z);
  auto ab = triangles.b() - triangles.a();
  auto ac = triangles.c() - triangles.a();
  auto h = direction.cross(ac);
  auto a = ab.dot(h);
  auto pass = std::abs(a) >= EPSILON;
  if (!pass) {
    return std::nullopt;
  }
  auto f = 1.f / a;
  auto s = origin - triangles.a();
  auto u = f * s.dot(h);
  pass = pass && (u >= ZERO) && (u <= ONE);
  if (!pass) {
    return std::nullopt;
  }
  auto q = s.cross(ab);
  auto v = f * direction.dot(q);
  pass = pass && (v >= ZERO) && (u + v <= ONE);
  if (!pass) {
    return std::nullopt;
  }
  auto t = f * ac.dot(q);
  pass = pass && (t > EPSILON) && (t <= EPSILON + tExit) &&
         (t + EPSILON >= tEntry);
  if (!pass) {
    return std::nullopt;
  }
  t = t.retain(pass, std::numeric_limits<float>::max());
  const auto idx = t.minIndex();
  if (idx < 0) {
    return std::nullopt;
  }
  const auto& triangle = *triangles.sources[idx];
  return Intersection(ray, t.v[idx], Vec3(u.v[idx], v.v[idx], 0.f), triangle,
                      triangle.material());
}

std::optional<Intersection> intersect(const Ray& ray,
                                      const AVX512Triangle& triangles,
                                      const float tEntry, const float tExit) {
  static const AVX512Float EPSILON = 1e-6f;
  static const AVX512Float ZERO = 0.f;
  static const AVX512Float ONE = 1.f;
  AVX512Vec3 origin(ray.origin->x, ray.origin->y, ray.origin->z);
  AVX512Vec3 direction(ray.direction->x, ray.direction->y, ray.direction->z);
  auto ab = triangles.b() - triangles.a();
  auto ac = triangles.c() - triangles.a();
  auto h = direction.cross(ac);
  auto a = ab.dot(h);
  auto pass = std::abs(a) >= EPSILON;
  if (!pass) {
    return std::nullopt;
  }
  auto f = 1.f / a;
  auto s = origin - triangles.a();
  auto u = f * s.dot(h);
  pass = pass && (u >= ZERO) && (u <= ONE);
  if (!pass) {
    return std::nullopt;
  }
  auto q = s.cross(ab);
  auto v = f * direction.dot(q);
  pass = pass && (v >= ZERO) && (u + v <= ONE);
  if (!pass) {
    return std::nullopt;
  }
  auto t = f * ac.dot(q);
  pass = pass && (t > EPSILON) && (t <= EPSILON + tExit) &&
         (t + EPSILON >= tEntry);
  if (!pass) {
    return std::nullopt;
  }
  t = t.retain(pass, std::numeric_limits<float>::max());
  const auto idx = t.minIndex();
  if (idx < 0) {
    return std::nullopt;
  }
  const auto& triangle = *triangles.sources[idx];
  return Intersection(ray, t.v[idx], Vec3(u.v[idx], v.v[idx], 0.f), triangle,
                      triangle.material());
}

std::optional<std::pair<float, float>> intersect(const Ray& ray,
                                                 const BoundingBox& aabb) {
  float tmin = (aabb.min()->x - ray.origin->x) / ray.direction->x;
  float tmax = (aabb.max()->x - ray.origin->x) / ray.direction->x;

  if (tmin > tmax) {
    std::swap(tmin, tmax);
  }

  float tymin = (aabb.min()->y - ray.origin->y) / ray.direction->y;
  float tymax = (aabb.max()->y - ray.origin->y) / ray.direction->y;

  if (tymin > tymax) {
    std::swap(tymin, tymax);
  }
  if (tymin > tmin) {
    tmin = tymin;
  }
  if (tymax < tmax) {
    tmax = tymax;
  }
  if (tmax <= tmin) {
    return std::nullopt;
  }

  float tzmin = (aabb.min()->z - ray.origin->z) / ray.direction->z;
  float tzmax = (aabb.max()->z - ray.origin->z) / ray.direction->z;

  if (tzmin > tzmax) {
    std::swap(tzmin, tzmax);
  }
  if (tzmin > tmin) {
    tmin = tzmin;
  }
  if (tzmax < tmax) {
    tmax = tzmax;
  }
  if (tmax <= tmin) {
    return std::nullopt;
  }
  return std::make_pair(tmin, tmax);
}
}  // namespace tinyrt
