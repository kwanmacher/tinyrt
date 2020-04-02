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

#include <iostream>

#include "core/obj.h"
#include "core/vec3.h"
#include "util/flag.h"

using namespace tinyrt;
using namespace std::literals;

constexpr char kOBJPath[] = "-obj";

int main(const int argc, const char** argv) {
  Flags<String<kOBJPath>> flags(argc, argv);

  Vec3 a(1.f, 2.f, 3.f);
  Vec3 b(3.f, 4.5, 6.f);
  std::cout << "Vec3 result=" << a.cross(b).normalize().dot(b) << std::endl;

  Obj cornellBox(flags.get<kOBJPath>());
  std::cout << "OBJ result=" << cornellBox << std::endl;
  return 0;
}