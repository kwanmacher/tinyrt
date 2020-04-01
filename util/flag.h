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

#include <string_view>
#include <tuple>
#include <utility>

namespace tinyrt {
class ConstString final {
  const char* p;
  const std::size_t sz;

 public:
  template <std::size_t N>
  constexpr ConstString(const char (&a)[N]) : p(a), sz(N - 1) {}

  constexpr char operator[](std::size_t n) const {
    return n < sz ? p[n] : throw std::out_of_range("");
  }
  constexpr std::size_t size() const { return sz; }

  //   bool operator==(char* other) const {
  //     auto* str = p;
  //     while (str && other && *(str++) == *(other++))
  //       ;
  //     return !str && !other;
  //   }
};

template <ConstString Name, typename T, T Default>
struct FlagSpec {
  using type = T;
  static constexpr ConstString kName = Name;
  static constexpr T kDefault = Default;
};

template <ConstString, ConstString Default>
using String = FlagSpec<Name, ConstString, Default>;

template <typename... T>
class Flag final {
 public:
  explicit Flag(int argc, char* argv[]) {
    for (auto i = 1; i < argc; i += 2) {
      set(argv[i], argv[i + 1], std::make_index_sequence<sizeof(T)>{});
    }
  }

  template <std::size_t... I>
  void set(char* argName, char* argValue, std::index_sequence<I...>) {
    (
        if (T::kName == argName) { std::get<I>(flags_) = argValue; }, ...);
  }

  void dump() { std::cout << std::get<0>(flags_); }

 private:
  std::tuple<T::type...> flags_;
};
}  // namespace tinyrt
