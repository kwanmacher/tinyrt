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

#include "core/intersect.h"
#include "util/async.h"

namespace tinyrt {
namespace {
static constexpr auto kMaxDepth = 15U;
static constexpr auto kEpsilon = 1e-4f;
static constexpr auto kTraversal = 1.f;
static constexpr auto kIntersect = 1.5f;

class DefaultNode final : public KdTree::Node {
 public:
  DefaultNode(const std::optional<SplitPlane>& split, KdTree::NodePtr left,
              KdTree::NodePtr right)
      : KdTree::Node(split, std::move(left), std::move(right)) {}

  explicit DefaultNode(std::vector<const Triangle*> triangles)
      : triangles_(std::move(triangles)) {}

  std::optional<Intersection> intersect(const Ray& ray, const float tEntry,
                                        const float tExit) const override {
    std::optional<Intersection> intersection;
    for (const auto* triangle : triangles_) {
      auto candidate = ::tinyrt::intersect(ray, *triangle);
      if (candidate &&
          (candidate->time >= tEntry - kEpsilon &&
           candidate->time <= tExit + kEpsilon) &&
          (!intersection || intersection->time > candidate->time)) {
        intersection = candidate;
      }
    }
    return intersection;
  }

 private:
  const std::vector<const Triangle*> triangles_;
};

class DefaultNodeFactory final : public KdTree::NodeFactory {
 public:
  KdTree::NodePtr createIntermediate(const std::optional<SplitPlane>& split,
                                     KdTree::NodePtr left,
                                     KdTree::NodePtr right) const override {
    return std::make_unique<DefaultNode>(split, std::move(left),
                                         std::move(right));
  }

  KdTree::NodePtr createLeaf(
      std::vector<const Triangle*> triangles) const override {
    return std::make_unique<DefaultNode>(std::move(triangles));
  }
};

static float bias(unsigned leftCount, unsigned rightCount, float leftProb,
                  float rightProb) {
  if ((leftCount == 0 || rightCount == 0) &&
      !(leftProb == 1 || rightProb == 1)) {
    return 0.8f;
  }
  return 1.0f;
}

static float cost(float leftProb, float rightProb, unsigned leftCount,
                  unsigned rightCount) {
  return (bias(leftCount, rightCount, leftProb, rightProb) *
          (kTraversal +
           kIntersect * (leftProb * leftCount + rightProb * rightCount)));
}

enum PlanarPlacement { LEFT, RIGHT, UNKNOWN };

static std::pair<float, PlanarPlacement> SAH(
    const unsigned dim, const float split, const BoundingBox& aabb,
    unsigned leftCount, unsigned rightCount, unsigned planarCount) {
  if (aabb.size()[dim] == 0) {
    return {std::numeric_limits<float>::max(), UNKNOWN};
  }
  const auto aabbs = aabb.cut(dim, split);
  const auto saTotal = aabb.area();
  const auto leftProb = aabbs.first.area() / saTotal;
  const auto rightProb = aabbs.second.area() / saTotal;
  if (leftProb == 0 || rightProb == 0) {
    return {std::numeric_limits<float>::max(), UNKNOWN};
  }
  const auto leftCost =
      cost(leftProb, rightProb, leftCount + planarCount, rightCount);
  const auto rightCost =
      cost(leftProb, rightProb, leftCount, rightCount + planarCount);
  if (leftCost < rightCost) {
    return {leftCost, LEFT};
  } else {
    return {rightCost, RIGHT};
  }
}

static bool terminate(int N, float minCv) { return (minCv > kIntersect * N); }

static KdTree::NodePtr build(std::vector<const Triangle*> triangles,
                             const BoundingBox& aabb, const unsigned depth,
                             const std::optional<SplitPlane>& prevSplit,
                             const KdTree::NodeFactory& nodeFactory) {
  if (triangles.empty()) {
    return nullptr;
  }
  if (triangles.size() <= 3 || depth >= kMaxDepth) {
    return nodeFactory.createLeaf(std::move(triangles));
  }
  const float maxFloat = std::numeric_limits<float>::max();
  float minCosts[3] = {maxFloat, maxFloat, maxFloat};
  std::pair<float, PlanarPlacement> splits[3];
  Async::submitN(
      [&triangles, &aabb, &minCosts, &splits](unsigned dim) {
        enum Type { ENDING, PLANAR, STARTING };
        using event_t = std::pair<float, Type>;
        std::vector<event_t> candidates;
        candidates.reserve(triangles.size() << 1);
        for (const auto* triangle : triangles) {
          auto clipped = triangle->aabb();
          clipped.clipTo(aabb);
          if (clipped.planar(dim)) {
            candidates.emplace_back(clipped.min()[dim], PLANAR);
          } else {
            candidates.emplace_back(clipped.min()[dim], STARTING);
            candidates.emplace_back(clipped.max()[dim], ENDING);
          }
        }
        std::sort(candidates.begin(), candidates.end(), [](auto& a, auto& b) {
          return (a.first < b.first) ||
                 (a.first == b.first && a.second < b.second);
        });

        unsigned leftCount = 0U;
        unsigned planarCount = 0U;
        unsigned rightCount = triangles.size();
        for (auto i = 0U; i < candidates.size(); ++i) {
          const auto& candidate = candidates[i];
          int starting = 0;
          int planar = 0;
          int ending = 0;
          while (i < candidates.size() &&
                 candidate.first == candidates[i].first &&
                 candidates[i].second == ENDING) {
            ++ending;
            ++i;
          }
          while (i < candidates.size() &&
                 candidate.first == candidates[i].first &&
                 candidates[i].second == PLANAR) {
            ++planar;
            ++i;
          }
          while (i < candidates.size() &&
                 candidate.first == candidates[i].first &&
                 candidates[i].second == STARTING) {
            ++starting;
            ++i;
          }
          planarCount = planar;
          rightCount -= planar + ending;
          const auto sah = SAH(dim, candidate.first, aabb, leftCount,
                               rightCount, planarCount);
          if (sah.first < minCosts[dim]) {
            minCosts[dim] = sah.first;
            splits[dim].first = candidate.first;
            splits[dim].second = sah.second;
          }
          leftCount += starting + planar;
          planarCount = 0;
        }
      },
      3);

  unsigned bestDim = -1;
  for (auto dim = 0U; dim < 3; ++dim) {
    if (minCosts[dim] < maxFloat &&
        (bestDim == -1 || minCosts[dim] < minCosts[bestDim])) {
      bestDim = dim;
    }
  }
  if (bestDim == -1 || terminate(triangles.size(), minCosts[bestDim])) {
    return nodeFactory.createLeaf(std::move(triangles));
  }
  const auto bestSplit = splits[bestDim];
  const SplitPlane split(bestDim, bestSplit.first);
  if (prevSplit && split == *prevSplit) {
    return nodeFactory.createLeaf(std::move(triangles));
  }
  const auto aabbs = aabb.cut(bestDim, bestSplit.first);
  std::vector<const Triangle*> leftTriangles;
  std::vector<const Triangle*> rightTriangles;
  for (const auto* triangle : triangles) {
    const auto aabb = triangle->aabb();
    if (aabb.min()[bestDim] == bestSplit.first &&
        aabb.max()[bestDim] == bestSplit.first) {
      if (bestSplit.second == LEFT) {
        leftTriangles.push_back(triangle);
      } else {
        rightTriangles.push_back(triangle);
      }
    } else {
      if (aabb.min()[bestDim] <= bestSplit.first) {
        leftTriangles.push_back(triangle);
      }
      if (aabb.max()[bestDim] >= bestSplit.first) {
        rightTriangles.push_back(triangle);
      }
    }
  }
  triangles.clear();
  auto leftChild = build(std::move(leftTriangles), aabbs.first, depth + 1,
                         split, nodeFactory);
  auto rightChild = build(std::move(rightTriangles), aabbs.second, depth + 1,
                          split, nodeFactory);
  return nodeFactory.createIntermediate(split, std::move(leftChild),
                                        std::move(rightChild));
}

static KdTree::NodePtr build(const Scene& scene,
                             std::unique_ptr<KdTree::NodeFactory> nodeFactory) {
  std::vector<const Triangle*> triangles;
  for (const auto& triangle : scene.triangles()) {
    triangles.push_back(triangle.get());
  }
  return build(std::move(triangles), scene.aabb(), 0, std::nullopt,
               *nodeFactory);
}
}  // namespace

KdTree::KdTree(const Scene& scene,
               std::unique_ptr<KdTree::NodeFactory> nodeFactory)
    : root_(build(scene, nodeFactory ? std::move(nodeFactory)
                                     : std::make_unique<DefaultNodeFactory>())),
      aabb_(scene.aabb()) {}
}  // namespace tinyrt
