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

#include "core/basic_tracer.h"

#include <exception>

#include "core/intersect.h"

namespace tinyrt {
void BasicTracer::initialize(const Scene& scene) { scene_ = &scene; }

std::optional<Intersection> BasicTracer::trace(const Ray& ray) {
  if (!scene_) {
    throw std::runtime_error("Must initialize with a scene first!");
  }
  std::optional<Intersection> intersection;
  for (const auto& triangle : scene_->triangles()) {
    auto candidate = intersect(ray, *triangle);
    if (candidate && (!intersection || intersection->time > candidate->time)) {
      intersection = candidate;
    }
  }
  return intersection;
}
}  // namespace tinyrt
