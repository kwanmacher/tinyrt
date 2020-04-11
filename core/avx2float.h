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

#include <immintrin.h>

#include "util/algorithm.h"

namespace tinyrt {
class AVX2Float final {
 public:
  /*implicit*/ AVX2Float(float const* source) : avx(_mm256_load_ps(source)) {}
  /*implicit*/ AVX2Float(const float source) : avx(_mm256_set1_ps(source)) {}
  /*implicit*/ AVX2Float(const __m256 source) : avx(source) {}

  AVX2Float operator+(const AVX2Float& other) const {
    return _mm256_add_ps(avx, other.avx);
  }

  AVX2Float operator-(const AVX2Float& other) const {
    return _mm256_sub_ps(avx, other.avx);
  }

  AVX2Float operator*(const AVX2Float& other) const {
    return _mm256_mul_ps(avx, other.avx);
  }

  AVX2Float operator/(const AVX2Float& other) const {
    return _mm256_div_ps(avx, other.avx);
  }

  AVX2Float& operator+=(const AVX2Float& other) {
    avx = _mm256_add_ps(avx, other.avx);
    return *this;
  }

  AVX2Float& operator-=(const AVX2Float& other) {
    avx = _mm256_sub_ps(avx, other.avx);
    return *this;
  }

  AVX2Float& operator*=(const AVX2Float& other) {
    avx = _mm256_mul_ps(avx, other.avx);
    return *this;
  }

  AVX2Float& operator/=(const AVX2Float& other) {
    avx = _mm256_div_ps(avx, other.avx);
    return *this;
  }

  AVX2Float operator>(const AVX2Float& other) const {
    return _mm256_cmp_ps(avx, other.avx, _CMP_GT_OQ);
  }

  AVX2Float operator>=(const AVX2Float& other) const {
    return _mm256_cmp_ps(avx, other.avx, _CMP_GE_OQ);
  }

  AVX2Float operator<(const AVX2Float& other) const {
    return _mm256_cmp_ps(avx, other.avx, _CMP_LT_OQ);
  }

  AVX2Float operator<=(const AVX2Float& other) const {
    return _mm256_cmp_ps(avx, other.avx, _CMP_LE_OQ);
  }

  AVX2Float operator==(const AVX2Float& other) const {
    return _mm256_cmp_ps(avx, other.avx, _CMP_EQ_OQ);
  }

  AVX2Float operator&&(const AVX2Float& other) const {
    return _mm256_and_ps(avx, other.avx);
  }

  AVX2Float operator||(const AVX2Float& other) const {
    return _mm256_or_ps(avx, other.avx);
  }

  AVX2Float operator>(const float other) const {
    return *this > AVX2Float(other);
  }

  AVX2Float operator>=(const float other) const {
    return *this >= AVX2Float(other);
  }

  AVX2Float operator<(const float other) const {
    return *this < AVX2Float(other);
  }

  AVX2Float operator<=(const float other) const {
    return *this <= AVX2Float(other);
  }

  AVX2Float operator==(const float other) const {
    return *this == AVX2Float(other);
  }

  bool operator!() const { return _mm256_testz_ps(avx, avx); }

  int8_t minIndex() const {
    __m256 vmin = _mm256_min_ps(avx, _mm256_castsi256_ps(_mm256_srli_epi32(
                                         _mm256_castps_si256(avx), 4)));

    vmin = _mm256_min_ps(vmin, _mm256_castsi256_ps(_mm256_srli_epi32(
                                   _mm256_castps_si256(avx), 2)));

    vmin = _mm256_min_ps(vmin, _mm256_castsi256_ps(_mm256_srli_epi32(
                                   _mm256_castps_si256(avx), 1)));

    __m256 vcmp =
        _mm256_cmp_ps(avx, _mm256_set1_ps(_mm256_cvtss_f32(vmin)), _CMP_EQ_OQ);

    uint32_t mask = _mm256_movemask_ps(vcmp);
    return mask == 0 ? -1 : __builtin_ctz(mask);
  }

  AVX2Float retain(const AVX2Float& mask, const float replace) const {
    return _mm256_blendv_ps(_mm256_set1_ps(replace), avx, mask.avx);
  }

  friend AVX2Float operator/(const float a, const AVX2Float& b) {
    return AVX2Float(a) / b;
  }

 public:
  alignas(16) union {
    __m256 avx;
    float v[8];
  };
};

template <>
const tinyrt::AVX2Float min<tinyrt::AVX2Float>(const tinyrt::AVX2Float& a,
                                               const tinyrt::AVX2Float& b);

template <>
const tinyrt::AVX2Float max<tinyrt::AVX2Float>(const tinyrt::AVX2Float& a,
                                               const tinyrt::AVX2Float& b);
}  // namespace tinyrt

namespace std {
tinyrt::AVX2Float sqrt(const tinyrt::AVX2Float& f);
tinyrt::AVX2Float abs(const tinyrt::AVX2Float& f);
}  // namespace std