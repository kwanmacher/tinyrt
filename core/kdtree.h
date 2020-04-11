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
#include "core/ray.h"
#include "core/scene.h"

namespace tinyrt {
class KdTree final {
 public:
  class Node;
  class NodeFactory;
  using NodePtr = std::unique_ptr<Node>;

 public:
  explicit KdTree(const Scene& scene,
                  std::unique_ptr<NodeFactory> nodeFactory = nullptr);

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

class KdTree::Node {
 public:
  Node() = default;
  Node(const std::optional<SplitPlane>& split, KdTree::NodePtr left,
       KdTree::NodePtr right)
      : split_(split), left_(std::move(left)), right_(std::move(right)) {}

  virtual ~Node() = default;

  const std::optional<SplitPlane>& split() const { return split_; }
  const KdTree::NodePtr& left() const { return left_; }
  const KdTree::NodePtr& right() const { return right_; }

  virtual std::optional<Intersection> intersect(const Ray& ray,
                                                const float tEntry,
                                                const float tExit) const = 0;

 private:
  const std::optional<SplitPlane> split_;
  const KdTree::NodePtr left_;
  const KdTree::NodePtr right_;
};

class KdTree::NodeFactory {
 public:
  virtual ~NodeFactory() = default;
  virtual KdTree::NodePtr createIntermediate(
      const std::optional<SplitPlane>& split, KdTree::NodePtr left,
      KdTree::NodePtr right) const = 0;
  virtual KdTree::NodePtr createLeaf(
      std::vector<const Triangle*> triangles) const = 0;
};
}  // namespace tinyrt
