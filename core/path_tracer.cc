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

#include "core/path_tracer.h"

#include <random>

namespace tinyrt {
namespace {
static constexpr auto kMaxDepth = 5U;

static Vec3 cosineSampledHemisphere(const Vec3& nx, const Vec3& ny,
                                    const Vec3& nz) {
  static thread_local std::mt19937 generator(std::random_device{}());
  std::uniform_real_distribution gen(0.f, 1.f);
  const float u1 = gen(generator);
  const float u2 = gen(generator);
  const float r = std::sqrtf(u1);
  const float theta = 2 * M_PI * u2;
  const float x = r * std::cosf(theta);
  const float y = r * std::sinf(theta);
  const Vec3 localSpaceVec(x, std::sqrtf(std::max(0.f, 1.f - u1)), y);
  return nx * localSpaceVec->x + ny * localSpaceVec->y + nz * localSpaceVec->z;
}
}  // namespace

Color PathTracer::trace(const RaySampler& raySampler,
                        const Intersecter& intersecter, const Scene& scene,
                        const Shader& shader,
                        const TraceOptions& options) const {
  if (options.directRays == 0) {
    return Color();
  }
  Color illumination;
  for (auto i = 0U; i < options.directRays; ++i) {
    illumination +=
        traceInternal(raySampler(), intersecter, scene, shader, options, 0);
  }
  return illumination / options.directRays;
}

Color PathTracer::traceInternal(const Ray& ray, const Intersecter& intersecter,
                                const Scene& scene, const Shader& shader,
                                const TraceOptions& options,
                                unsigned depth) const {
  if (depth >= kMaxDepth) {
    return Color();
  }
  const auto intersection = intersecter.intersect(ray);
  if (!intersection) {
    return options.background;
  }
  Color directIllumination;
  for (const auto& light : scene.lights()) {
    Vec3 localIllumination = shader.shade(*intersection, *light);
    const unsigned shadowSamples = options.shadowRays;
    if (shadowSamples > 0 && !intersection->material->light() &&
        !localIllumination.zero()) {
      const auto shadowRayOrigin =
          intersection->position + intersection->normal * 1e-4f;
      unsigned occlusion = 0U;
      for (auto i = 0U; i < shadowSamples; ++i) {
        const auto lightVec = light->aabb.random() - intersection->position;
        const Ray shadowRay(shadowRayOrigin, lightVec);
        const auto shadowCheck = intersecter.intersect(shadowRay);
        if (shadowCheck && shadowCheck->time < lightVec.norm() - 1e-3f) {
          occlusion++;
        }
      }
      localIllumination *= (shadowSamples - occlusion) * 1.f / shadowSamples;
    }
    directIllumination += localIllumination;
  }

  Color indirectIllumination;
  if (options.indirectRays > 0) {
    const auto basis = intersection->normal.basis();
    auto indirectOptions = options;
    indirectOptions.indirectRays = options.indirectRays / 2;
    indirectOptions.shadowRays = 1;

    for (auto i = 0U; i < options.indirectRays; ++i) {
      Ray indirectRay(
          intersection->position + intersection->normal * 1e-4f,
          cosineSampledHemisphere(std::get<0>(basis), std::get<1>(basis),
                                  std::get<2>(basis)));
      indirectIllumination +=
          traceInternal(indirectRay, intersecter, scene, shader,
                        indirectOptions, depth + 1) *
          indirectRay.direction.dot(intersection->normal);
    }
    const Color brdf = intersection->material->diffuse;
    indirectIllumination =
        indirectIllumination * brdf * 2.f / options.indirectRays;
  }
  return directIllumination / M_PI + indirectIllumination;
}
}  // namespace tinyrt
