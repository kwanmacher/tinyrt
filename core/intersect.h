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

#include "core/tracer.h"
#include "core/triangle.h"

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
  // TODO(kaikai): Normal interpolation.
  return Intersection(ray, t, triangle.a().normal, triangle.material());
}
}  // namespace tinyrt
