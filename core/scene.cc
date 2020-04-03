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

#include "core/scene.h"

#include <algorithm>
#include <iterator>

namespace tinyrt {
namespace {
static Vertex makeVertex(const std::vector<Vec3>& vertices,
                         const std::vector<Vec3>& texcoords,
                         const std::vector<Vec3>& normals,
                         const std::array<int32_t, 3>& indices) {
  return Vertex(
      vertices[indices[VERTEX]],
      indices[TEXCOORD] >= 0 ? &texcoords[indices[TEXCOORD]] : nullptr,
      normals[indices[NORMAL]]);
}

static std::vector<std::unique_ptr<Triangle>> makeTriangles(
    const std::vector<Vec3>& vertices, const std::vector<Vec3>& texcoords,
    const std::vector<Vec3>& normals, const std::vector<Material>& materials,
    const std::vector<triangle_indices_t>& triangles) {
  std::vector<std::unique_ptr<Triangle>> out;
  std::transform(
      triangles.begin(), triangles.end(), std::back_inserter(out),
      [&](const triangle_indices_t& indices) {
        std::array<Vertex, 3> triangleVertices{
            makeVertex(vertices, texcoords, normals, indices.first[0]),
            makeVertex(vertices, texcoords, normals, indices.first[1]),
            makeVertex(vertices, texcoords, normals, indices.first[2]),
        };
        return std::make_unique<Triangle>(triangleVertices,
                                          materials[indices.second]);
      });
  return out;
}
}  // namespace

Scene::Scene(std::vector<Vec3> vertices, std::vector<Vec3> texcoords,
             std::vector<Vec3> normals, std::vector<Material> materials,
             const std::vector<triangle_indices_t>& triangles)
    : vertices_(std::move(vertices)),
      texcoords_(std::move(texcoords)),
      normals_(std::move(normals)),
      materials_(std::move(materials)),
      triangles_(makeTriangles(vertices_, texcoords_, normals_, materials_,
                               triangles)) {}

const std::vector<std::unique_ptr<Triangle>>& Scene::triangles() const {
  return triangles_;
}
}  // namespace tinyrt
