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
struct AVX512FMask final {
  __mmask16 mask;

  /*implicit*/ AVX512FMask(const __mmask16 source) : mask(source) {}

  AVX512FMask operator&&(const AVX512FMask& other) const {
    return _kand_mask16(mask, other.mask);
  }

  AVX512FMask operator||(const AVX512FMask& other) const {
    return _kor_mask16(mask, other.mask);
  }

  bool operator!() const { return mask == 0; }
};

class AVX512Float final {
 public:
  /*implicit*/ AVX512Float(float const* source) : avx(_mm512_load_ps(source)) {}
  /*implicit*/ AVX512Float(const float source) : avx(_mm512_set1_ps(source)) {}
  /*implicit*/ AVX512Float(const __m512 source) : avx(source) {}

  AVX512Float operator+(const AVX512Float& other) const {
    return _mm512_add_ps(avx, other.avx);
  }

  AVX512Float operator-(const AVX512Float& other) const {
    return _mm512_sub_ps(avx, other.avx);
  }

  AVX512Float operator*(const AVX512Float& other) const {
    return _mm512_mul_ps(avx, other.avx);
  }

  AVX512Float operator/(const AVX512Float& other) const {
    return _mm512_div_ps(avx, other.avx);
  }

  AVX512Float& operator+=(const AVX512Float& other) {
    avx = _mm512_add_ps(avx, other.avx);
    return *this;
  }

  AVX512Float& operator-=(const AVX512Float& other) {
    avx = _mm512_sub_ps(avx, other.avx);
    return *this;
  }

  AVX512Float& operator*=(const AVX512Float& other) {
    avx = _mm512_mul_ps(avx, other.avx);
    return *this;
  }

  AVX512Float& operator/=(const AVX512Float& other) {
    avx = _mm512_div_ps(avx, other.avx);
    return *this;
  }

  AVX512FMask operator>(const AVX512Float& other) const {
    return _mm512_cmp_ps_mask(avx, other.avx, _CMP_GT_OQ);
  }

  AVX512FMask operator>=(const AVX512Float& other) const {
    return _mm512_cmp_ps_mask(avx, other.avx, _CMP_GE_OQ);
  }

  AVX512FMask operator<(const AVX512Float& other) const {
    return _mm512_cmp_ps_mask(avx, other.avx, _CMP_LT_OQ);
  }

  AVX512FMask operator<=(const AVX512Float& other) const {
    return _mm512_cmp_ps_mask(avx, other.avx, _CMP_LE_OQ);
  }

  AVX512FMask operator==(const AVX512Float& other) const {
    return _mm512_cmp_ps_mask(avx, other.avx, _CMP_EQ_OQ);
  }

  AVX512FMask operator>(const float other) const {
    return *this > AVX512Float(other);
  }

  AVX512FMask operator>=(const float other) const {
    return *this >= AVX512Float(other);
  }

  AVX512FMask operator<(const float other) const {
    return *this < AVX512Float(other);
  }

  AVX512FMask operator<=(const float other) const {
    return *this <= AVX512Float(other);
  }

  AVX512FMask operator==(const float other) const {
    return *this == AVX512Float(other);
  }

  int8_t minIndex() const {
    float vmin = _mm512_reduce_min_ps(avx);
    __mmask16 mask = _mm512_cmp_ps_mask(avx, _mm512_set1_ps(vmin), _CMP_EQ_OQ);
    return mask == 0 ? -1 : __builtin_ctz(mask);
  }

  AVX512Float retain(const AVX512FMask& mask, const float replace) const {
    return _mm512_mask_blend_ps(mask.mask, _mm512_set1_ps(replace), avx);
  }

  friend AVX512Float operator/(const float a, const AVX512Float& b) {
    return AVX512Float(a) / b;
  }

 public:
  alignas(16) union {
    __m512 avx;
    float v[16];
  };
};

template <>
const tinyrt::AVX512Float min<tinyrt::AVX512Float>(
    const tinyrt::AVX512Float& a, const tinyrt::AVX512Float& b);

template <>
const tinyrt::AVX512Float max<tinyrt::AVX512Float>(
    const tinyrt::AVX512Float& a, const tinyrt::AVX512Float& b);
}  // namespace tinyrt

namespace std {
tinyrt::AVX512Float sqrt(const tinyrt::AVX512Float& f);
tinyrt::AVX512Float abs(const tinyrt::AVX512Float& f);
}  // namespace std