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

#include "core/intersect.h"
#include "core/kdtree.h"
#include "core/simd_triangle.h"

namespace tinyrt {
template <typename TVec3>
class SimdKdTreeNode final : public KdTree::Node {
 public:
  SimdKdTreeNode(const std::optional<SplitPlane>& split, KdTree::NodePtr left,
                 KdTree::NodePtr right)
      : KdTree::Node(split, std::move(left), std::move(right)) {}

  explicit SimdKdTreeNode(std::vector<const Triangle*> triangles)
      : simdTriangles_(buildSimdTriangles<TVec3>(triangles)) {}

  std::optional<Intersection> intersect(const Ray& ray, const float tEntry,
                                        const float tExit) const override {
    std::optional<Intersection> intersection;
    for (const auto& triangle : simdTriangles_) {
      auto candidate = ::tinyrt::intersect(ray, triangle, tEntry, tExit);
      if (candidate &&
          (!intersection || intersection->time > candidate->time)) {
        intersection = candidate;
      }
    }
    return intersection;
  }

 private:
  const std::vector<SimdTriangle<TVec3>> simdTriangles_;
};

template <typename TVec3>
class SimdKdTreeNodeFactory final : public KdTree::NodeFactory {
 public:
  KdTree::NodePtr createIntermediate(const std::optional<SplitPlane>& split,
                                     KdTree::NodePtr left,
                                     KdTree::NodePtr right) const override {
    return std::make_unique<SimdKdTreeNode<TVec3>>(split, std::move(left),
                                                   std::move(right));
  }

  virtual KdTree::NodePtr createLeaf(
      std::vector<const Triangle*> triangles) const override {
    return std::make_unique<SimdKdTreeNode<TVec3>>(std::move(triangles));
  }
};
}  // namespace tinyrt
