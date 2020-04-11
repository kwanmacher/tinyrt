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

#include <algorithm>

#include "core/avx2float.h"
#include "core/avx512float.h"
#include "core/triangle.h"

namespace tinyrt {
template <typename TVec3>
struct SimdTriangle final {
  using vec3_t = TVec3;

  const std::array<TVec3, 3> vertices;
  const std::vector<const Triangle*> sources;

  explicit SimdTriangle(const std::array<TVec3, 3>& vertices,
                        const std::vector<const Triangle*>& sources)
      : vertices(vertices), sources(sources) {}

  const TVec3& a() const { return vertices[0]; }
  const TVec3& b() const { return vertices[1]; }
  const TVec3& c() const { return vertices[2]; }
};

using AVX2Vec3 = Vec3T<AVX2Float>;
using AVX2Triangle = SimdTriangle<AVX2Vec3>;

using AVX512Vec3 = Vec3T<AVX512Float, AVX512FMask>;
using AVX512Triangle = SimdTriangle<AVX512Vec3>;

template <typename TVec3>
std::vector<SimdTriangle<TVec3>> buildSimdTriangles(
    const std::vector<const Triangle*>& triangles) {
  static constexpr auto kWidth =
      sizeof(typename TVec3::float_t) / sizeof(float);
  std::vector<SimdTriangle<TVec3>> ret;
  alignas(32) float buffer[3][kWidth];
  std::array<TVec3, 3> vertices;
  std::vector<const Triangle*> sourceTriangles(kWidth);
  for (auto t = 0UL; t < triangles.size(); t += kWidth) {
    for (auto i = 0U; i < 3; ++i) {
      for (auto j = 0U; j < kWidth; ++j) {
        const auto& vertex =
            triangles[std::min(t + j, triangles.size() - 1)]->vertices()[i];
        for (auto k = 0U; k < 3; ++k) {
          buffer[k][j] = vertex.coord[k];
        }
      }
      vertices[i] = TVec3((float const*)&buffer[0], (float const*)&buffer[1],
                          (float const*)&buffer[2]);
    }
    std::copy_n(triangles.begin() + t, std::min(triangles.size() - t, kWidth),
                sourceTriangles.begin());
    ret.emplace_back(vertices, sourceTriangles);
  }
  return ret;
}
}  // namespace tinyrt
