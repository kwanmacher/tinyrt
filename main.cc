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

#include <fstream>
#include <iostream>

#include "core/basic_intersecter.h"
#include "core/camera.h"
#include "core/obj.h"
#include "core/phong_shader.h"
#include "core/stream.h"
#include "util/flag.h"

using namespace tinyrt;
using namespace std::literals;

constexpr char kOBJPath[] = "-obj";
constexpr char kOutPath[] = "-out";

int main(const int argc, const char** argv) {
  Flags<String<kOBJPath>, String<kOutPath>> flags(argc, argv);

  Obj cornellBox(flags.get<kOBJPath>());
  std::cout << "OBJ file loaded: " << cornellBox << std::endl;

  auto scene = std::move(cornellBox).moveToScene();
  std::cout << "Scene created: " << *scene << std::endl;

  Camera camera(Vec3(0.f, 1.f, 3.93f), Vec3(0.f, 0.f, -1.f),
                Vec3(0.f, 1.f, 0.f), 39.f);
  BasicIntersecter intersecter;
  PhongShader shader;
  intersecter.initialize(*scene);

  const unsigned width = 320;
  const unsigned height = 320;
  Vec3 result[width][height];

  auto rayGenerator = camera.adapt(width, height);
  for (auto i = 0; i < width; ++i) {
    for (auto j = 0; j < height; ++j) {
      const auto ray = rayGenerator(i, j);
      const auto intersection = intersecter.trace(ray);
      if (intersection) {
        for (const auto& light : scene->lights()) {
          result[i][j] = shader.shade(*intersection, *light);
        }
      }
    }
    std::cout << "Finished " << i << "/" << width << std::endl;
  }

  std::ofstream ppm(flags.get<kOutPath>());
  ppm << "P3" << std::endl;
  ppm << width << " " << height << std::endl;
  ppm << "255" << std::endl;
  int total = 0;
  for (auto i = 0; i < height; ++i) {
    for (auto j = 0; j < width; ++j) {
      auto c = result[j][i] * 255;
      for (auto k = 0; k < 3; k++) {
        int clamped = std::max(std::min((int)c[k], 255), 0);
        ppm << clamped << " ";
      }
      if (++total % 5 == 0) {
        ppm << std::endl;
      }
    }
  }
  ppm.flush();
  ppm.close();
  return 0;
}