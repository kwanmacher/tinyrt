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

#include "core/constants.h"
#include "core/shader.h"

namespace tinyrt {
class PhongShader : public Shader {
 public:
  Color shade(const Intersection& intersection,
              const Light& light) const override {
    const auto* material = intersection.material;
    if (material->light()) {
      return material->emittance * material->ambient;
    }
    const auto l = (light.aabb.center() - intersection.position).normalize();
    Color lumination;
    if (material->illuminationModel & Material::DIFFUSE) {
      lumination = lumination + light.material.emittance * material->diffuse *
                                    std::max(l.dot(intersection.normal), 0.f);
    }
    if (material->illuminationModel & Material::SPECULAR &&
        !light.material.specular.small()) {
      const auto r = -l.reflect(intersection.normal);
      const auto v = -intersection.ray.direction;
      lumination = lumination + light.material.emittance * material->specular *
                                    std::pow(std::max(r.dot(v), 0.f),
                                             material->specularExponent);
    }
    return lumination;
  }
};
}  // namespace tinyrt
