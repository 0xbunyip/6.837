#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "Camera.h"
#include "Image.h"
#include "RayTracer.h"
#include "SceneParser.h"
#include "util.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"

void printProgress(int i, int j, int imWidth, int imHeight) {
  int numPixels = imWidth * imHeight / 10;
  int pixel = i * imHeight + j;
  if (pixel > 1 && pixel / numPixels > (pixel - 1) / numPixels) {
    std::cout << "Done " << pixel * 10.0 / numPixels << "%\n";
  }
}

int main(int argc, char *argv[]) {
  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a5").
  int inputIdx = -1, sizeIdx = -1, outputIdx = -1, depthIdx = -1,
      bouncesIdx = -1;
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
    } else if (arg == "-bounces") {
      bouncesIdx = argNum + 1;
    }
  }
  assert(inputIdx > 0);
  assert(sizeIdx > 0);
  assert(outputIdx > 0);

  auto scenePath = argv[inputIdx];
  auto imWidth = atoi(argv[sizeIdx]);
  auto imHeight = atoi(argv[sizeIdx + 1]);
  auto outputPath = argv[outputIdx];
  int bounces = 0;
  if (bouncesIdx > 0) {
    bounces = atoi(argv[bouncesIdx]);
  }

  // First, parse the scene using SceneParser.
  SceneParser* scene = new SceneParser(scenePath);
  auto camera = scene->getCamera();
  auto tracer = std::make_unique<RayTracer>(scene, bounces);

  Image image(imWidth, imHeight);

  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.
  float aspectRatio = imWidth * 1.0 / imHeight;
  for (int i = 0; i < imWidth; ++i) {
    float x = (-1.0 + i * 2.0 / (imWidth - 1.0)) * aspectRatio;
    // if (i != imWidth / 2) continue;
    for (int j = 0; j < imHeight; ++j) {
      // if (j != imHeight / 2 + 10)
      //   continue;

      float y = -1.0 + j * 2.0 / (imHeight - 1.0);
      // LOG(x, y);
      auto point = Vector2f(x, y);
      auto ray = camera->generateRay(point);

      Hit hit;
      auto color = tracer->traceRay(ray, camera->getTMin(), 0, 0.0, hit);
      image.SetPixel(i, j, color);
      // LOG(i, j, color);

      printProgress(i, j, imWidth, imHeight);
    }
  }

  image.SaveImage(outputPath);
  return 0;
}
