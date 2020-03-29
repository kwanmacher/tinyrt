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

#include "scene.h"

#include <algorithm>

namespace tinyrt {
namespace {
static std::vector<Triangle> makeTriangles(
    const Scene& scene, const std::vector<Triangle::indices_t>& triangles) {
  std::vector<Triangle> out(triangles.size());
  std::transform(triangles.begin(), triangles.end(), out.begin(),
                 [scene](const Triangle::indices_t& indices) {
                   return Triangle(scene, indices);
                 });
  return out;
}
}  // namespace

Scene::Scene(std::vector<Vec3> vertices, std::vector<Vec3> texcoords,
             std::vector<Vec3> normals,
             const std::vector<Triangle::indices_t>& triangles)
    : vertices_(std::move(vertices)),
      texcoords_(std::move(texcoords_)),
      normals_(std::move(normals)),
      triangles_(makeTriangles(*this, triangles)) {}
}  // namespace tinyrt
