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

#include "core/avx512float.h"

namespace tinyrt {
template <>
const tinyrt::AVX512Float min<tinyrt::AVX512Float>(
    const tinyrt::AVX512Float& a, const tinyrt::AVX512Float& b) {
  return _mm512_min_ps(a.avx, b.avx);
}

template <>
const tinyrt::AVX512Float max<tinyrt::AVX512Float>(
    const tinyrt::AVX512Float& a, const tinyrt::AVX512Float& b) {
  return _mm512_max_ps(a.avx, b.avx);
}
}  // namespace tinyrt

namespace std {
tinyrt::AVX512Float sqrt(const tinyrt::AVX512Float& f) {
  return _mm512_sqrt_ps(f.avx);
}

tinyrt::AVX512Float abs(const tinyrt::AVX512Float& f) {
  return _mm512_abs_ps(f.avx);
}
}  // namespace std
