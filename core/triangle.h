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

#include <array>

#include "core/vec3.h"

namespace tinyrt {
struct Vertex final {
  const Vec3& coord;
  const Vec3* texcoord;
  const Vec3& normal;

  Vertex(const Vec3& coord, const Vec3* texcoord, const Vec3& normal)
      : coord(coord), texcoord(texcoord), normal(normal) {}
};

class Triangle final {
 public:
  explicit Triangle(const std::array<Vertex, 3>& vertices);
  Triangle(const Triangle&) = delete;
  Triangle& operator=(const Triangle&) = delete;

  inline const Vertex& a() const;
  inline const Vertex& b() const;
  inline const Vertex& c() const;

 private:
  const std::array<Vertex, 3> vertices_;
};
}  // namespace tinyrt
