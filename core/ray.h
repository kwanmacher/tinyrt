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

#include <optional>

#include "core/scene.h"

namespace tinyrt {
struct Ray {
  Vec3 origin;
  Vec3 direction;

  Ray(const Vec3& origin, const Vec3& direction)
      : origin(origin), direction(direction.normalize()) {}
};

struct Intersection {
  Ray ray;
  float time;
  Vec3 position;
  Vec3 uv;
  const Triangle* triangle;
  const Material* material;

  Intersection(const Ray& ray, const float time, const Vec3& uv,
               const Triangle& triangle, const Material& material)
      : ray(ray),
        time(time),
        position(ray.origin + ray.direction * time),
        uv(uv),
        triangle(&triangle),
        material(&material) {}

  const Vec3& normal() const {
    if (!normal_) {
      normal_ = triangle->a().normal * (1 - uv->x - uv->y) +
                triangle->b().normal * uv->x + triangle->c().normal * uv->y;
    }
    return *normal_;
  }

 private:
  // Computed on-demand.
  mutable std::optional<Vec3> normal_;
};
}  // namespace tinyrt
