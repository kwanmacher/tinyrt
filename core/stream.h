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

#include "core/obj.h"
#include "core/scene.h"
#include "core/triangle.h"
#include "core/vec3.h"

namespace tinyrt {
std::ostream& operator<<(std::ostream& os, const Vertex& vertex) {
  os << "Vertex{coord=" << vertex.coord << ", texcoord=";
  if (vertex.texcoord) {
    os << *vertex.texcoord;
  } else {
    os << "none";
  }
  os << ", normal=" << vertex.normal << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Triangle& triangle) {
  os << "Triangle{a=" << triangle.a() << ", b=" << triangle.b()
     << ", c=" << triangle.c() << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Scene& scene) {
  os << "Scene{" << std::endl;
  for (auto i = 0; i < scene.triangles_.size(); ++i) {
    os << "#" << i << ": " << *scene.triangles_.at(i) << std::endl;
  }
  os << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Obj& obj) {
  os << "Obj{vertices=" << obj.vertices_.size()
     << ", texcoords=" << obj.texcoords_.size()
     << ", normals=" << obj.normals_.size() << ", faces=" << obj.faces_.size()
     << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
  os << "Vec3{x=" << vec->x << ",y=" << vec->y << ",z=" << vec->z << "}";
  return os;
}
}  // namespace tinyrt
