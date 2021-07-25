#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "Camera.h"
#include "Image.h"
#include "SceneParser.h"
#include "util.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"

int main(int argc, char *argv[]) {
  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  int inputIdx = -1, sizeIdx = -1, outputIdx = -1, depthIdx = -1;
  for (int argNum = 1; argNum < argc; ++argNum) {
    std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    std::string arg = argv[argNum];

    if (arg == "-input") {
      inputIdx = argNum + 1;
    } else if (arg == "-size") {
      sizeIdx = argNum + 1;
    } else if (arg == "-output") {
      outputIdx = argNum + 1;
    } else if (arg == "-depth") {
      depthIdx = argNum + 1;
    }
  }
  assert(inputIdx > 0);
  assert(sizeIdx > 0);
  assert(outputIdx > 0);
  assert(depthIdx > 0);

  auto scenePath = argv[inputIdx];
  auto imWidth = atoi(argv[sizeIdx]);
  auto imHeight = atoi(argv[sizeIdx + 1]);
  auto outputPath = argv[outputIdx];

  // First, parse the scene using SceneParser.
  auto scene = SceneParser(scenePath);
  auto camera = scene.getCamera();
  auto group = scene.getGroup();

  int numLights = scene.getNumLights();
  auto ambient = scene.getAmbientLight();
  auto background = scene.getBackgroundColor();

  Image image(imWidth, imHeight);

  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.
  float aspectRatio = imWidth * 1.0 / imHeight;
  int numPixels = imWidth * imHeight / 10;
  for (int i = 0; i < imWidth; ++i) {
    float x = (-1.0 + i * 2.0 / (imWidth - 1.0)) * aspectRatio;
    for (int j = 0; j < imHeight; ++j) {
      float y = -1.0 + j * 2.0 / (imHeight - 1.0);
      auto point = Vector2f(x, y);
      auto ray = camera->generateRay(point);

      Vector3f color = background;
      Hit hit;
      if (group->intersect(ray, hit, camera->getTMin())) {
        auto p = ray.pointAtParameter(hit.getT());
        auto material = hit.getMaterial();

        color = ambient;
        for (int k = 0; k < numLights; ++k) {
          auto light = scene.getLight(k);
          float distanceToLight = 0; // Unused.

          Vector3f dirToLight, lightColor;
          light->getIllumination(p, dirToLight, lightColor, distanceToLight);

          color += material->Shade(ray, hit, dirToLight, lightColor);
        }
      }
      image.SetPixel(i, j, color);

      int pixel = i * imHeight + j;
      if (pixel > 1 && pixel / numPixels > (pixel - 1) / numPixels) {
        std::cout << "Done " << pixel * 10.0 / numPixels << "%\n";
      }
    }
  }

  image.SaveImage(outputPath);
  return 0;
}

