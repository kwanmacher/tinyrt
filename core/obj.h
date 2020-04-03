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
#include <string>
#include <vector>

#include "core/scene.h"
#include "core/vec3.h"

namespace tinyrt {
class Obj final {
 public:
  explicit Obj(const std::string& path);
  Obj(const Obj&) = delete;
  Obj& operator=(const Obj&) = delete;

  std::unique_ptr<Scene> toScene() const&;
  std::unique_ptr<Scene> moveToScene() &&;

  friend std::ostream& operator<<(std::ostream& os, const Obj& obj);

 public:
  using face_indices_t =
      std::pair<std::vector<std::array<int32_t, 3>>, uint32_t>;

 private:
  std::vector<Vec3> vertices_;
  std::vector<Vec3> texcoords_;
  std::vector<Vec3> normals_;
  std::vector<Material> materials_;
  std::vector<face_indices_t> faces_;
};
}  // namespace tinyrt
