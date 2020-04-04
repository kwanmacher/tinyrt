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

#include <filesystem>
#include <fstream>
#include <sstream>
#include <tuple>
#include <unordered_map>

#include "core/constants.h"

namespace tinyrt {
namespace {
static auto loadMtl(const std::string& path, std::vector<Material>& materials) {
  std::unordered_map<std::string, uint32_t> matIndexMap;
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  }
  std::string line;
  Material* material = nullptr;
  while (std::getline(file, line)) {
    std::istringstream lineStream(line);
    std::string op;
    if (!(lineStream >> op)) {
      continue;
    }
    switch (op[0]) {
      case '#':
        break;
      case 'n': {
        std::string matName;
        lineStream >> matName;
        if (matIndexMap.find(matName) != matIndexMap.end()) {
          throw std::runtime_error("Duplicate material name");
        }
        material = &(materials.emplace_back());
        matIndexMap.emplace(matName, materials.size() - 1);
      } break;
      case 'K': {
        if (!material) {
          throw std::runtime_error("No current material!");
        }
        Vec3 value;
        lineStream >> value->x >> value->y >> value->z;
        switch (op[1]) {
          case 'a':
            material->ambient = value;
            break;
          case 'd':
            material->diffuse = value;
            break;
          case 's':
            material->specular = value;
            break;
          case 'e':
            material->emittance = value;
            break;
          default:
            break;
        }
      } break;
      case 'd': {
        if (!material) {
          throw std::runtime_error("No current material!");
        }
        std::string next;
        lineStream >> next;
        float dissolve;
        if (next == "-halo") {
          lineStream >> dissolve;
        } else {
          dissolve = std::stof(next);
        }
        material->dissolve = dissolve;
      } break;
      case 'i': {
        if (!material) {
          throw std::runtime_error("No current material!");
        }
        int illum;
        lineStream >> illum;
        switch (illum) {
          case 0:
          case 1:
            material->illuminationModel = Material::DIFFUSE;
            break;
          case 2:
            material->illuminationModel =
                static_cast<Material::IlluminationModel>(Material::DIFFUSE |
                                                         Material::SPECULAR);
            break;
          default:
            break;
        }
      } break;
      case 's':
      case 'N': {
        if (!material) {
          throw std::runtime_error("No current material!");
        }
        float value;
        lineStream >> value;
        switch (op[1]) {
          case 's':
            material->specularExponent = value;
            break;
          case 'i':
            material->refractionIndex = value;
            break;
          case 'h':
            material->sharpness = value;
            break;
          default:
            break;
        }
      } break;
      default:
        break;
    }
  }
  return matIndexMap;
}

static auto loadObj(const std::string& path) {
  std::vector<Vec3> vectors[3];
  std::vector<Obj::face_indices_t> faces;
  std::vector<Material> materials;
  std::unordered_map<std::string, uint32_t> matIndexMap;
  std::vector<light_t> lights;

  // Fallback material.
  materials.emplace_back();
  uint32_t material = 0;
  bool light = false;

  std::ifstream file(path);
  const std::filesystem::path fsPath(path);
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
        face.second = material;
        std::string v;
        while (lineStream >> v) {
          std::istringstream vertexStream(v);
          size_t idx = 0;
          std::string component;
          auto& vertex = *face.first.insert(face.first.cend(), {-1, -1, -1});
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
              if (idx == VERTEX && light) {
                lights.back().first.add(vectors[idx][value]);
              }
            }
            ++idx;
          }
        }
      } break;
      case 'm': {
        std::string mtl;
        lineStream >> mtl;
        matIndexMap = loadMtl(fsPath.parent_path() / mtl, materials);
      } break;
      case 'u': {
        std::string mtl;
        lineStream >> mtl;
        material =
            matIndexMap.find(mtl) != matIndexMap.end() ? matIndexMap[mtl] : 0;
        light = materials[material].light();
        if (light) {
          lights.emplace_back(BoundingBox(), material);
        }
      } break;
      default:
        break;
    }
  }
  return std::make_tuple(vectors[VERTEX], vectors[TEXCOORD], vectors[NORMAL],
                         materials, faces, lights);
}

static std::unique_ptr<Scene> createScene(
    std::vector<Vec3> vertices, std::vector<Vec3> texcoords,
    std::vector<Vec3> normals, std::vector<Material> materials,
    std::vector<Obj::face_indices_t> faces, std::vector<light_t> lights) {
  std::vector<triangle_indices_t> triangles;
  for (auto& face : faces) {
    if (face.first.size() < 3) {
      throw std::length_error("A face must have at least 3 vertices!");
    } else {
      const auto& v0 = vertices[face.first[0][VERTEX]];
      for (auto i = 1; i < face.first.size() - 1; ++i) {
        if (face.first[0][NORMAL] < 0 || face.first[i][NORMAL] < 0 ||
            face.first[i + 1][NORMAL] < 0) {
          const auto& vi = vertices[face.first[i][VERTEX]];
          const auto& vi1 = vertices[face.first[i + 1][VERTEX]];
          normals.emplace_back((vi - v0).cross(vi1 - v0).normalize());
          face.first[0][NORMAL] = face.first[i][NORMAL] =
              face.first[i + 1][NORMAL] = normals.size() - 1;
        }
        triangles.push_back(
            {{face.first[0], face.first[i], face.first[i + 1]}, face.second});
      }
    }
  }
  return std::make_unique<Scene>(std::move(vertices), std::move(texcoords),
                                 std::move(normals), std::move(materials),
                                 std::move(triangles), std::move(lights));
}
}  // namespace

Obj::Obj(const std::string& path) {
  std::tie(vertices_, texcoords_, normals_, materials_, faces_, lights_) =
      loadObj(path);
}

std::unique_ptr<Scene> Obj::toScene() const& {
  return createScene(vertices_, texcoords_, normals_, materials_, faces_,
                     lights_);
}

std::unique_ptr<Scene> Obj::moveToScene() && {
  return createScene(std::move(vertices_), std::move(texcoords_),
                     std::move(normals_), std::move(materials_),
                     std::move(faces_), std::move(lights_));
}
}  // namespace tinyrt
