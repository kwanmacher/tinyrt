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

#include <chrono>
#include <fstream>
#include <future>
#include <random>

#include "core/basic_intersecter.h"
#include "core/camera.h"
#include "core/kdtree_intersecter.h"
#include "core/obj.h"
#include "core/path_tracer.h"
#include "core/phong_shader.h"
#include "core/ray_tracer.h"
#include "core/simd_kdtree_node.h"
#include "core/stream.h"
#include "util/async.h"
#include "util/capabilities.h"
#include "util/flag.h"
#include "util/log.h"

using namespace tinyrt;
using namespace std::literals;

constexpr char kOBJPath[] = "-obj";
constexpr char kOutPath[] = "-out";

std::unique_ptr<KdTree::NodeFactory> createKdTreeNodeFactory() {
  if (supportsAvx512f()) {
    LOG(INFO) << "Enabled AVX512F support";
    return std::make_unique<SimdKdTreeNodeFactory<AVX512Vec3>>();
  } else if (supportsAvx2()) {
    LOG(INFO) << "Enabled AVX2 support";
    return std::make_unique<SimdKdTreeNodeFactory<AVX2Vec3>>();
  }
  LOG(INFO) << "No AVX support detected, fallback to default";
  return nullptr;
}

int main(const int argc, const char** argv) {
  Flags<String<kOBJPath>, String<kOutPath>> flags(argc, argv);

  Obj cornellBox(flags.get<kOBJPath>());
  LOG(INFO) << "OBJ file loaded: " << cornellBox;

  auto scene = std::move(cornellBox).moveToScene();
  LOG(INFO) << "Scene created: " << *scene;

  Camera camera(Vec3(0.f, .8f, 3.93f), Vec3(0.f, 0.f, -1.f),
                Vec3(0.f, 1.f, 0.f), 32.f);
  KdTreeIntersecter intersecter(createKdTreeNodeFactory());
  PhongShader shader;
  PathTracer rayTracer;
  intersecter.initialize(*scene);

  const unsigned width = 640;
  const unsigned height = 508;
  const unsigned block = 8;
  const auto totalBlocks =
      ((width + block - 1) / block) * ((height + block - 1) / block);
  Vec3 result[width][height];
  std::atomic_int completed;
  std::promise<void> promise;

  const TraceOptions options{
      .directRays = 200,
      .indirectRays = 1,
      .shadowRays = 1,
  };
  auto rayGenerator = camera.adapt(width, height);

  LOG(INFO) << "Rendering started.";
  const auto begin = std::chrono::steady_clock::now();
  for (auto i = 0; i < width; i += block) {
    for (auto j = 0; j < height; j += block) {
      Async::submit([&, i, j] {
        static thread_local std::mt19937 generator;
        std::uniform_real_distribution gen(0.f, 1.f);
        unsigned kTarget = std::min(width, i + block);
        unsigned lTarget = std::min(height, j + block);
        for (auto k = i; k < kTarget; ++k) {
          for (auto l = j; l < lTarget; ++l) {
            const RaySampler raySampler([&] {
              return rayGenerator(k + gen(generator), l + gen(generator));
            });
            result[k][l] = rayTracer.trace(raySampler, intersecter, *scene,
                                           shader, options);
          }
        }
        const auto completedBlocks = ++completed;
        if (completedBlocks % (totalBlocks / 100) == 0) {
          LOG(INFO) << "Finished " << completedBlocks << "/" << totalBlocks;
        }
        if (completedBlocks == totalBlocks) {
          LOG(INFO) << "Completed!";
          promise.set_value();
        }
      });
    }
  }
  promise.get_future().wait();
  LOG(INFO) << "Rendering finished. Time elapsed="
            << std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::steady_clock::now() - begin)
                   .count()
            << "s.";

  std::ofstream ppm(flags.get<kOutPath>());
  ppm << "P3";
  ppm << width << " " << height;
  ppm << "255";
  int total = 0;
  for (auto i = 0; i < height; ++i) {
    for (auto j = 0; j < width; ++j) {
      auto c = result[j][i] * 255;
      for (auto k = 0; k < 3; k++) {
        int clamped = std::max(std::min((int)c[k], 255), 0);
        ppm << clamped << " ";
      }
      if (++total % 5 == 0) {
        ppm;
      }
    }
  }
  ppm.flush();
  ppm.close();
  return 0;
}