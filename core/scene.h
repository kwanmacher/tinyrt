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

#include <memory>
#include <vector>

#include "core/light.h"
#include "core/triangle.h"
#include "core/vec3.h"

namespace tinyrt {
using triangle_indices_t =
    std::pair<std::array<std::array<int32_t, 3>, 3>, uint32_t>;
using light_t = std::pair<BoundingBox, uint32_t>;

enum Index { VERTEX, TEXCOORD, NORMAL };

class Scene final {
 public:
  Scene(std::vector<Vec3> vertices, std::vector<Vec3> texcoords,
        std::vector<Vec3> normals, std::vector<Material> materials,
        const std::vector<triangle_indices_t>& triangles,
        const std::vector<light_t>& lights);
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;

  const std::vector<std::unique_ptr<Triangle>>& triangles() const;
  const std::vector<std::unique_ptr<Light>>& lights() const;

  friend std::ostream& operator<<(std::ostream& os, const Scene& scene);

 private:
  const std::vector<Vec3> vertices_;
  const std::vector<Vec3> texcoords_;
  const std::vector<Vec3> normals_;
  const std::vector<Material> materials_;
  const std::vector<std::unique_ptr<Triangle>> triangles_;
  const std::vector<std::unique_ptr<Light>> lights_;
};
}  // namespace tinyrt
