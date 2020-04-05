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

#include "core/kdtree.h"

#include <future>
#include <limits>

#include "util/async.h"

namespace tinyrt {
namespace {
static constexpr auto kMaxDepth = 15U;
static constexpr auto kEpsilon = 1e-4f;

static KdTree::NodePtr build(std::vector<const Triangle*> triangles,
                             const BoundingBox& aabb, const unsigned depth) {
  if (triangles.empty()) {
    return nullptr;
  }
  if (triangles.size() <= 3 || depth >= kMaxDepth) {
    return std::make_unique<KdTree::Node>(aabb, nullptr, nullptr,
                                          std::move(triangles));
  }
  const float maxFloat = std::numeric_limits<float>::max();
  float minCosts[3] = {maxFloat, maxFloat, maxFloat};
  float splits[3];
  Async::submitN(
      [&triangles, &aabb, &minCosts, &splits](unsigned dim) {
        std::vector<std::pair<float, bool>> candidates;
        const auto aabbMin = aabb.min()[dim];
        const auto aabbMax = aabb.max()[dim];
        for (const auto* triangle : triangles) {
          const auto extent = triangle->extent(dim);
          if (extent.first - aabbMin > kEpsilon &&
              aabbMax - extent.first > kEpsilon) {
            candidates.emplace_back(extent.first, true);
          }
          if (extent.second - aabbMin > kEpsilon &&
              aabbMax - extent.second > kEpsilon) {
            candidates.emplace_back(extent.second, false);
          }
        }
        if (candidates.empty()) {
          return;
        }
        std::sort(candidates.begin(), candidates.end(),
                  [](auto& a, auto& b) { return a.first < b.first; });

        unsigned leftCount = 0U;
        unsigned rightCount = triangles.size();
        auto candidate = candidates.begin();
        do {
          if (leftCount > 0 && rightCount > 0) {
            const auto cut = aabb.cut(dim, candidate->first);
            auto cost =
                cut.first.area() * leftCount + cut.second.area() * rightCount;
            if (cost < minCosts[dim]) {
              minCosts[dim] = cost;
              splits[dim] = candidate->first;
            }
          }

          if (candidate->second) {
            ++leftCount;
          } else {
            --rightCount;
          }

          auto old = candidate++;
          while (candidate != candidates.end() &&
                 candidate->first - old->first < kEpsilon) {
            if (candidate->second) {
              ++leftCount;
            } else {
              --rightCount;
            }
            old = candidate++;
          }
        } while (candidate != candidates.end());
      },
      3);

  unsigned bestDim = -1;
  for (auto dim = 0U; dim < 3; ++dim) {
    if (minCosts[dim] < maxFloat &&
        (bestDim == -1 || minCosts[dim] < minCosts[bestDim])) {
      bestDim = dim;
    }
  }
  if (bestDim == -1) {
    return std::make_unique<KdTree::Node>(aabb, nullptr, nullptr,
                                          std::move(triangles));
  }
  const float bestSplit = splits[bestDim];

  const auto aabbs = aabb.cut(bestDim, bestSplit);
  std::vector<const Triangle*> leftTriangles;
  std::vector<const Triangle*> rightTriangles;
  for (const auto* triangle : triangles) {
    const auto extent = triangle->extent(bestDim);
    if (extent.second >= bestSplit) {
      rightTriangles.push_back(triangle);
    }
    if (extent.first <= bestSplit) {
      leftTriangles.push_back(triangle);
    }
  }

  auto leftChild = build(std::move(leftTriangles), aabbs.first, depth + 1);
  auto rightChild = build(std::move(rightTriangles), aabbs.second, depth + 1);
  return std::make_unique<KdTree::Node>(aabb, std::move(leftChild),
                                        std::move(rightChild),
                                        std::vector<const Triangle*>());
}

static KdTree::NodePtr build(const Scene& scene) {
  BoundingBox aabb;
  std::vector<const Triangle*> triangles;
  for (const auto& triangle : scene.triangles()) {
    for (const auto& vertex : triangle->vertices()) {
      aabb.add(vertex.coord);
    }
    triangles.push_back(triangle.get());
  }
  return build(std::move(triangles), aabb, 0);
}
}  // namespace

KdTree::KdTree(const Scene& scene) : root_(build(scene)) {}
}  // namespace tinyrt
