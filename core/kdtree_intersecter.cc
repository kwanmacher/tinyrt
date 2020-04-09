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
#include <stack>

#include "core/intersect.h"

namespace tinyrt {

static constexpr auto kEpsilon = 1e-5f;

void KdTreeIntersecter::initialize(const Scene& scene) {
  kdTree_ = std::make_unique<KdTree>(scene);
}

std::optional<Intersection> KdTreeIntersecter::intersect(const Ray& ray) const {
  if (!kdTree_) {
    throw std::runtime_error("Must initialize with a scene first!");
  }
  if (const auto aabbIntersect = ::tinyrt::intersect(ray, kdTree_->aabb())) {
    return intersectInternal(ray, kdTree_->root(), aabbIntersect->first,
                             aabbIntersect->second);
  }
  return std::nullopt;
}

std::optional<Intersection> KdTreeIntersecter::intersectInternal(
    const Ray& ray, const KdTree::NodePtr& node, const float t0,
    const float t1) const {
  using node_visitor_t = std::tuple<KdTree::Node*, float, float>;
  std::stack<node_visitor_t> stack;
  stack.emplace(node.get(), t0, t1);
  while (!stack.empty()) {
    KdTree::Node* currentNode;
    float tEntry, tExit;
    std::tie(currentNode, tEntry, tExit) = stack.top();
    stack.pop();
    while (auto& split = currentNode->split()) {
      const auto dim = split->dim;
      const auto pos = split->split;
      const auto ts = (pos - ray.origin[dim]) / ray.direction[dim];
      const bool leftFirst = ray.direction[dim] > 0;
      KdTree::Node* near =
          leftFirst ? currentNode->left().get() : currentNode->right().get();
      KdTree::Node* far =
          leftFirst ? currentNode->right().get() : currentNode->left().get();
      if (ts > tExit) {
        currentNode = near;
      } else if (ts < tEntry) {
        currentNode = far;
      } else {
        stack.emplace(far, ts, tExit);
        currentNode = near;
        tExit = ts;
      }
    }
    const auto intersection = currentNode->intersect(ray, tEntry, tExit);
    if (intersection) {
      return intersection;
    }
  }
  return std::nullopt;
}
}  // namespace tinyrt
