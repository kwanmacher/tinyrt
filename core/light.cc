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

#include "core/light.h"

namespace tinyrt {
float Light::intensity(const Vec3& point) const {
  const auto& size = aabb.size();
  const auto distance = point - aabb.center();
  const auto direction = distance.normalize();
  const auto areax = size->y * size->z;
  const auto areay = size->x * size->z;
  const auto areaz = size->x * size->y;
  const auto area = areax + areay + areaz;
  const auto unattenuatedIntensity =
      (std::abs(direction->x) * areax + std::abs(direction->y) * areay +
       std::abs(direction->z) * areaz) /
      area;
  return unattenuatedIntensity / (4 * M_PI * distance.norm());
}
}  // namespace tinyrt
