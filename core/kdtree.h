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

#include <optional>

#include "core/bounding_box.h"
#include "core/scene.h"

namespace tinyrt {
class KdTree final {
 public:
  struct Node;
  using NodePtr = std::unique_ptr<Node>;

 public:
  explicit KdTree(const Scene& scene);

  const NodePtr& root() const { return root_; }
  const BoundingBox& aabb() const { return aabb_; }

 private:
  NodePtr const root_;
  const BoundingBox aabb_;
};

struct SplitPlane {
  const unsigned dim;
  const float split;

  SplitPlane(const unsigned dim, const float split) : dim(dim), split(split) {}

  bool operator==(const SplitPlane& other) const {
    return dim == other.dim && split == other.split;
  }
};

struct KdTree::Node {
  const std::optional<SplitPlane> split;
  const KdTree::NodePtr left;
  const KdTree::NodePtr right;
  const std::vector<const Triangle*> triangles;

  Node(const std::optional<SplitPlane>& split, KdTree::NodePtr left,
       KdTree::NodePtr right)
      : split(split), left(std::move(left)), right(std::move(right)) {}

  explicit Node(std::vector<const Triangle*> triangles)
      : triangles(std::move(triangles)) {}
};
}  // namespace tinyrt
