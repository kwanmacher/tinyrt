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

#include "core/kdtree_intersecter.h"

#include <exception>

#include "core/intersect.h"

namespace tinyrt {

void KdTreeIntersecter::initialize(const Scene& scene) {
  kdTree_ = std::make_unique<KdTree>(scene);
}

std::optional<Intersection> KdTreeIntersecter::intersect(const Ray& ray) const {
  if (!kdTree_) {
    throw std::runtime_error("Must initialize with a scene first!");
  }
  return intersectInternal(ray, kdTree_->root());
}

std::optional<Intersection> KdTreeIntersecter::intersectInternal(
    const Ray& ray, const KdTree::NodePtr& node) const {
  if (!node) {
    return std::nullopt;
  }
  if (!::tinyrt::intersect(ray, node->aabb)) {
    return std::nullopt;
  }
  if (node->left || node->right) {
    const auto leftHit = intersectInternal(ray, node->left);
    const auto rightHit = intersectInternal(ray, node->right);
    if (leftHit && rightHit) {
      return leftHit->time < rightHit->time ? leftHit : rightHit;
    } else {
      return leftHit ?: rightHit;
    }
  } else {
    std::optional<Intersection> intersection;
    for (const auto* triangle : node->triangles) {
      auto candidate = ::tinyrt::intersect(ray, *triangle);
      if (candidate &&
          (!intersection || intersection->time > candidate->time)) {
        intersection = candidate;
      }
    }
    return intersection;
  }
}
}  // namespace tinyrt
