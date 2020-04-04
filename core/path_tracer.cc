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

#include "core/constants.h"

namespace tinyrt {
namespace {
static constexpr auto kMaxDepth = 3U;
static constexpr auto kSamples = 400U;

static auto& randomEngine() {
  static thread_local std::mt19937 generator(std::random_device{}());
  return generator;
}

static std::vector<Vec3>& precomputedHemisphereVecs() {
  static std::optional<std::vector<Vec3>> storage;
  if (!storage) {
    std::uniform_real_distribution gen(0.f, 1.f);
    auto& engine = randomEngine();
    storage.emplace();
    for (auto i = 0U; i < kSamples; ++i) {
      const float u1 = gen(engine);
      const float u2 = gen(engine);
      const float r = std::sqrtf(u1);
      const float theta = 2 * kPI * u2;
      const float x = r * std::cosf(theta);
      const float y = r * std::sinf(theta);
      storage->push_back(Vec3(x, y, std::sqrtf(std::max(0.f, 1.f - u1))));
    }
  }
  return *storage;
}

static Vec3 hemisphereVec() {
  std::uniform_real_distribution gen(0.f, 1.f);
  auto& engine = randomEngine();
  const float u1 = gen(engine);
  const float u2 = gen(engine);
  const float r = std::sqrtf(u1);
  const float theta = 2 * kPI * u2;
  const float x = r * std::cosf(theta);
  const float y = r * std::sinf(theta);
  return Vec3(x, y, std::sqrtf(std::max(0.f, 1.f - u1)));
}

static Vec3 cosineSampledHemisphere(const Vec3& nx, const Vec3& ny,
                                    const Vec3& n) {
  static const auto& precomputed = precomputedHemisphereVecs();
  static auto& engine = randomEngine();
  std::uniform_int_distribution<unsigned> gen(0U, kSamples);
  Vec3 vec = hemisphereVec();  // precomputed[gen(engine)];
  Vec3 transformedVec = nx * vec->x + ny * vec->y + n * vec->z;
  if (transformedVec.dot(n) < 0) {
    transformedVec = -transformedVec;
  }
  return transformedVec;
}
}  // namespace

Color PathTracer::trace(const RaySampler& raySampler,
                        const Intersecter& intersecter, const Scene& scene,
                        const Shader& shader,
                        const TraceOptions& options) const {
  return traceInternal(raySampler, intersecter, scene, shader, options, 0);
}

Color PathTracer::traceInternal(const RaySampler& raySampler,
                                const Intersecter& intersecter,
                                const Scene& scene, const Shader& shader,
                                const TraceOptions& options,
                                unsigned depth) const {
  if (depth >= kMaxDepth) {
    return Color();
  }
  const unsigned directSamples = std::max(options.directRays, 1U);
  unsigned hit = 0;
  std::optional<Intersection> intersection;
  Color illumination;
  Color directIllumination;
  for (auto i = 0U; i < directSamples; ++i) {
    const auto sampleIntersection = intersecter.intersect(raySampler());
    if (!sampleIntersection) {
      continue;
    }
    ++hit;
    if (sampleIntersection && !intersection) {
      intersection = sampleIntersection;
    }
    for (const auto& light : scene.lights()) {
      Vec3 localIllumination = shader.shade(*sampleIntersection, *light);
      const unsigned shadowSamples = options.shadowRays;
      if (shadowSamples > 0 && !sampleIntersection->material->light() &&
          !localIllumination.zero()) {
        const auto shadowRayOrigin =
            sampleIntersection->position + sampleIntersection->normal * 1e-4f;
        unsigned occlusion = 0U;
        for (auto j = 0U; j < shadowSamples; ++j) {
          const auto lightVec =
              light->aabb.random() - sampleIntersection->position;
          const Ray shadowRay(shadowRayOrigin, lightVec);
          const auto shadowCheck = intersecter.intersect(shadowRay);
          if (shadowCheck && shadowCheck->time < lightVec.norm() - 1e-3f) {
            occlusion++;
          }
        }
        localIllumination *= (shadowSamples - occlusion) * 1.f / shadowSamples;
      }
      directIllumination = directIllumination + localIllumination;
    }
  }
  if (!intersection) {
    return Color();
  }

  directIllumination /= directSamples;
  illumination = directIllumination;
  Color indirectIllumination;
  if (options.indirectRays > 0) {
    const float samplingBias = hit * 1.f / directSamples;
    const auto basis = intersection->normal.basis();
    const RaySampler indirectRaySampler([&] {
      return Ray(intersection->position + intersection->normal * 1e-4f,
                 cosineSampledHemisphere(std::get<0>(basis), std::get<1>(basis),
                                         std::get<2>(basis)));
    });
    auto indirectOptions = options;
    indirectOptions.directRays = options.indirectRays;
    indirectOptions.indirectRays = options.indirectRays;  // / 2;
    indirectOptions.shadowRays = 1;

    const Color brdf = intersection->material->diffuse;
    const Color incoming = traceInternal(indirectRaySampler, intersecter, scene,
                                         shader, indirectOptions, depth + 1);
    indirectIllumination = incoming * 2.f * brdf * samplingBias;
  }
  illumination = illumination / kPI + indirectIllumination;
  return illumination;
}
}  // namespace tinyrt
