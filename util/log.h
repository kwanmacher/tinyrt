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

#include <iostream>
#include <sstream>
#include <thread>

namespace tinyrt {
constexpr int32_t checkBaseNameIndex(const int32_t basename_idx) {
  return basename_idx >= 0 ? basename_idx
                           : (throw std::logic_error("compile-time basename"));
}

constexpr int32_t baseNameIndex(const char* const path, const int32_t index = 0,
                                const int32_t slash_index = -1) {
  return checkBaseNameIndex(
      path[index]
          ? (path[index] == '/' ? baseNameIndex(path, index + 1, index)
                                : baseNameIndex(path, index + 1, slash_index))
          : (slash_index + 1));
}

#define __FILENAME__ (&__FILE__[baseNameIndex(__FILE__)])

#define LOG(level) \
  ::tinyrt::Log(::tinyrt::level, __FILENAME__, __LINE__).stream()

enum Level { INFO, WARNING, ERROR };

class Log final {
 public:
  Log(const Level level, const std::string fileName, const int line) {
    switch (level) {
      case INFO:
        stream_ << "I";
        break;
      case WARNING:
        stream_ << "W";
        break;
      case ERROR:
        stream_ << "E";
        break;
    }
    stream_ << " " << std::this_thread::get_id() << " " << fileName << ":"
            << line << ": ";
  }

  ~Log() { std::cout << stream_.str() << std::endl; }

  std::ostringstream& stream() { return stream_; }

 private:
  std::ostringstream stream_;
};
}  // namespace tinyrt
