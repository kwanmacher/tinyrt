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

#include "core/camera.h"

namespace tinyrt {

const double kPI = std::acos(-1);

Camera::Camera(const Vec3& position, const Vec3& direction, const Vec3& up,
               const float fov)
    : position_(position),
      direction_(direction.normalize()),
      up_(up.normalize()),
      fov_(fov) {}

std::function<Ray(unsigned, unsigned)> Camera::adapt(
    const unsigned width, const unsigned height) const {
  const auto aspectRatio = width * 1.f / height;
  const auto left = up_.cross(direction_);
  const auto fov = direction_.norm() * std::tan(fov_ / 360 * kPI);
  const auto adaptedUp = up_ * fov;
  const auto adaptedLeft = left * (fov * aspectRatio);
  const auto topLeft = position_ + direction_ + adaptedUp + adaptedLeft;
  const auto xbasis = -adaptedLeft * 2.f / width;
  const auto ybasis = -adaptedUp * 2.f / height;
  return
      [position = position_, topLeft, xbasis, ybasis](unsigned x, unsigned y) {
        return Ray(position, topLeft + xbasis * x + ybasis * y - position);
      };
}
}  // namespace tinyrt
