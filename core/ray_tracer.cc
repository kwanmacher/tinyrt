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

#include "core/ray_tracer.h"

namespace tinyrt {
Color RayTracer::trace(const Ray& ray, const Intersecter& intersecter,
                       const Scene& scene, const Shader& shader) const {
  Color illumination;
  const auto intersection = intersecter.intersect(ray);
  if (intersection) {
    for (const auto& light : scene.lights()) {
      Vec3 illum = shader.shade(*intersection, *light);
      if (!illum.small()) {
        const auto lightVec = light->aabb.center() - intersection->position;
        const Ray shadowRay(
            intersection->position + intersection->normal * 1e-4f, lightVec);
        const auto shadowCheck = intersecter.intersect(shadowRay);
        if (shadowCheck && shadowCheck->time < lightVec.norm() - 1e-3f) {
          illum = Vec3();
        }
      }
      illumination = illumination + illum;
    }
  }
  return illumination;
}
}  // namespace tinyrt
