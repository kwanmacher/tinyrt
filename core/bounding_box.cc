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

#include "core/bounding_box.h"

#include <chrono>
#include <limits>
#include <random>

namespace tinyrt {
namespace {
static constexpr auto kMinFloat = std::numeric_limits<float>::min();
static constexpr auto kMaxFloat = std::numeric_limits<float>::max();
}  // namespace

BoundingBox::BoundingBox()
    : BoundingBox(Vec3(kMaxFloat, kMaxFloat, kMaxFloat),
                  Vec3(kMinFloat, kMinFloat, kMinFloat)) {}

BoundingBox::BoundingBox(const Vec3& min, const Vec3& max)
    : min_(min), max_(max), size_(max - min), center_((min_ + max_) / 2.f) {}

bool BoundingBox::contains(const Vec3& point) const {
  return point >= min_ && point <= max_;
}

const Vec3& BoundingBox::center() const { return center_; }

const Vec3& BoundingBox::size() const { return size_; }

const Vec3& BoundingBox::min() const { return min_; }
const Vec3& BoundingBox::max() const { return max_; }

float BoundingBox::area() const {
  return (size_->x * size_->y + size_->x * size_->z + size_->y * size_->z) * 2;
}

Vec3 BoundingBox::random() const {
  if (min_.same(max_)) {
    return min_;
  }
  static thread_local std::mt19937 generator;
  std::uniform_real_distribution gen(0.f, 1.f);
  Vec3 pos = size_;
  for (auto i = 0; i < 3; ++i) {
    pos[i] *= gen(generator);
  }
  return min_ + pos;
}

bool BoundingBox::planar(const unsigned dim) const {
  const float kEpsilon = 1e-6f;
  return std::abs(size_[dim]) < kEpsilon;
}

std::pair<BoundingBox, BoundingBox> BoundingBox::cut(unsigned dim,
                                                     float location) const {
  location = std::max(std::min(location, max_[dim]), min_[dim]);
  auto leftMax = max_;
  leftMax[dim] = location;
  auto rightMin = min_;
  rightMin[dim] = location;
  return {BoundingBox(min_, leftMax), BoundingBox(rightMin, max_)};
}

void BoundingBox::add(const Vec3& vec) {
  min_ = min_.min(vec);
  max_ = max_.max(vec);
  size_ = max_ - min_;
  center_ = (min_ + max_) / 2.f;
}

void BoundingBox::clipTo(const BoundingBox& other) {
  for (auto i = 0U; i < 3; ++i) {
    if (other.min_[i] > min_[i]) {
      min_[i] = other.min_[i];
    }
    if (other.max_[i] < max_[i]) {
      max_[i] = other.max_[i];
    }
  }
}
}  // namespace tinyrt
