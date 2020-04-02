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

#include <optional>
#include <sstream>
#include <string_view>
#include <tuple>

namespace tinyrt {
constexpr char kEmpty[] = "";

template <const char* Name, typename T, T Default>
struct Flag {
  using type = T;
  static constexpr auto kName = std::string_view(Name);
  static constexpr T kDefault = Default;
  std::optional<T> value;
};

template <const char* Name, const char* Default = kEmpty>
struct String final : public Flag<Name, const char*, Default> {
  static const auto* convert(const char* arg) { return arg; }
};

template <const char* Name, int Default = 0>
struct Int final : public Flag<Name, int, Default> {
  static int convert(const char* arg) { return std::atoi(arg); }
};

template <const char* Name, bool Default = false>
struct Bool final : public Flag<Name, bool, Default> {
  static bool convert(const char* arg) {
    bool ret;
    std::istringstream argStream(arg);
    argStream >> std::boolalpha >> ret;
    return ret;
  }
};

template <typename... T>
class Flags final {
 public:
  explicit Flags(const int argc, const char* argv[]) {
    for (auto i = 1; i < argc; i += 2) {
      set(argv[i], argv[i + 1], std::make_index_sequence<sizeof...(T)>{});
    }
  }

  template <const char* Arg, std::size_t I = 0>
  auto get() const {
    using arg_t = typename std::tuple_element<I, decltype(flags_)>::type;
    if constexpr (arg_t::kName == Arg) {
      const auto& value = std::get<I>(flags_).value;
      return value ? *value : arg_t::kDefault;
    } else {
      return get<Arg, I + 1>();
    }
  }

 private:
  template <std::size_t... I>
  void set(const char* argName, const char* argValue,
           std::index_sequence<I...>) {
    (
        [&] {
          if (T::kName == argName) {
            std::get<I>(flags_).value = T::convert(argValue);
          }
          return 0;
        }(),
        ...);
  }

 private:
  std::tuple<T...> flags_;
};
}  // namespace tinyrt
