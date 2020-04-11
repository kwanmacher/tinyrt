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

#include "core/avx2float.h"

namespace tinyrt {
template <>
const tinyrt::AVX2Float min<tinyrt::AVX2Float>(const tinyrt::AVX2Float& a,
                                               const tinyrt::AVX2Float& b) {
  return _mm256_min_ps(a.avx, b.avx);
}

template <>
const tinyrt::AVX2Float max<tinyrt::AVX2Float>(const tinyrt::AVX2Float& a,
                                               const tinyrt::AVX2Float& b) {
  return _mm256_max_ps(a.avx, b.avx);
}
}  // namespace tinyrt

namespace std {
tinyrt::AVX2Float sqrt(const tinyrt::AVX2Float& f) {
  return _mm256_sqrt_ps(f.avx);
}

tinyrt::AVX2Float abs(const tinyrt::AVX2Float& f) {
  static const __m256 kSignMask =
      _mm256_castsi256_ps(_mm256_set1_epi32(1 << 31));
  return _mm256_andnot_ps(kSignMask, f.avx);
}
}  // namespace std
