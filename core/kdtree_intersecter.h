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

#include "core/intersecter.h"
#include "core/kdtree.h"

namespace tinyrt {
class KdTreeIntersecter final : public Intersecter {
 public:
  explicit KdTreeIntersecter(
      std::unique_ptr<KdTree::NodeFactory> nodeFactory = nullptr)
      : nodeFactory_(std::move(nodeFactory)) {}

  void initialize(const Scene& scene) override;
  std::optional<Intersection> intersect(const Ray& ray) const override;

 private:
  std::optional<Intersection> intersectInternal(const Ray& ray,
                                                const KdTree::NodePtr& node,
                                                const float t0,
                                                const float t1) const;

 private:
  std::unique_ptr<KdTree::NodeFactory> nodeFactory_;
  std::unique_ptr<KdTree> kdTree_;
};
}  // namespace tinyrt
