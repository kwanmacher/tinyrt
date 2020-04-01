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
static std::vector<Triangle> makeTriangles(
    const Scene& scene, const std::vector<Triangle::indices_t>& triangles) {
  std::vector<Triangle> out;
  std::transform(triangles.begin(), triangles.end(), std::back_inserter(out),
                 [&scene](const Triangle::indices_t& indices) {
                   return Triangle(scene, indices);
                 });
  return out;
}
}  // namespace

Triangle::Triangle(const Scene& scene, const indices_t& indices)
    : scene_(scene), indices_(indices) {}

const Vec3& Triangle::vertex(const size_t idx) const {
  return scene_.vertices_[indices_[idx][VERTEX]];
}

const Vec3& Triangle::texcoord(const size_t idx) const {
  return scene_.texcoords_[indices_[idx][TEXCOORD]];
}

const Vec3& Triangle::normal(const size_t idx) const {
  return scene_.normals_[indices_[idx][NORMAL]];
}

Scene::Scene(std::vector<Vec3> vertices, std::vector<Vec3> texcoords,
             std::vector<Vec3> normals,
             const std::vector<Triangle::indices_t>& triangles)
    : vertices_(std::move(vertices)),
      texcoords_(std::move(texcoords)),
      normals_(std::move(normals)),
      triangles_(makeTriangles(*this, triangles)) {}
}  // namespace tinyrt
