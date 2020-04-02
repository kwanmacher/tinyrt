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

#include "core/obj.h"

#include <fstream>
#include <sstream>
#include <tuple>

namespace tinyrt {
namespace {
static auto loadObj(const std::string& path) {
  enum __VECTYPE { VERTEX, TEXCOORD, NORMAL };
  std::vector<Vec3> vectors[3];
  std::vector<Obj::face_indices_t> faces;

  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  }
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream lineStream(line);
    std::string op;
    if (!(lineStream >> op)) {
      continue;
    }
    switch (op[0]) {
      case '#':
        break;
      case 'v': {
        Vec3 vec;
        lineStream >> vec->x >> vec->y;
        if (op.size() == 1) {
          lineStream >> vec->z;
          vectors[VERTEX].push_back(vec);
        } else if (op[1] == 'n') {
          lineStream >> vec->z;
          vectors[NORMAL].push_back(vec);
        } else if (op[1] == 't') {
          vectors[TEXCOORD].push_back(vec);
        }
      } break;
      case 'f': {
        auto& face = faces.emplace_back();
        std::string vertex;
        while (lineStream >> vertex) {
          std::istringstream vertexStream(vertex);
          size_t idx = 0;
          std::string component;
          auto& vertex = *face.insert(face.cend(), {-1});
          while (std::getline(vertexStream, component, '/')) {
            if (!component.empty()) {
              int value = std::stoi(component);
              if (value < 0) {
                value = vectors[idx].size() + value;
              } else {
                --value;
              }
              if (value < 0 || value >= vectors[idx].size()) {
                throw std::out_of_range("Face index out-of-range!");
              }
              vertex[idx] = value;
            }
            ++idx;
          }
        }
      } break;
      default:
        break;
    }
  }
  return std::make_tuple(vectors[VERTEX], vectors[TEXCOORD], vectors[NORMAL],
                         faces);
}

static std::unique_ptr<Scene> createScene(
    std::vector<Vec3> vertices, std::vector<Vec3> texcoords,
    std::vector<Vec3> normals, const std::vector<Obj::face_indices_t>& faces) {
  std::vector<Triangle::indices_t> triangles;
  for (const auto& face : faces) {
    if (face.size() < 3) {
      throw std::length_error("A face must have at least 3 vertices!");
    } else {
      for (auto i = 1; i < face.size() - 1; ++i) {
        triangles.push_back({face[0], face[i], face[i + 1]});
      }
    }
  }
  return std::make_unique<Scene>(std::move(vertices), std::move(texcoords),
                                 std::move(normals), std::move(triangles));
}
}  // namespace

Obj::Obj(const std::string& path) {
  std::tie(vertices_, texcoords_, normals_, faces_) = loadObj(path);
}

std::unique_ptr<Scene> Obj::toScene() const& {
  return createScene(vertices_, texcoords_, normals_, faces_);
}

std::unique_ptr<Scene> Obj::moveToScene() && {
  return createScene(std::move(vertices_), std::move(texcoords_),
                     std::move(normals_), std::move(faces_));
}

std::ostream& operator<<(std::ostream& os, const Obj& obj) {
  os << "Obj{vertices=" << obj.vertices_.size()
     << ", texcoords=" << obj.texcoords_.size()
     << ", normals=" << obj.normals_.size() << ", faces=" << obj.faces_.size()
     << "}";
  return os;
}
}  // namespace tinyrt
