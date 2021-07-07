#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"

int main(int argc, char *argv[]) {
  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  int inputIdx, sizeIdx, outputIdx, depthIdx;
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

  // First, parse the scene using SceneParser.
  auto scene = SceneParser(scenePath);

  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.
  for (int i = 0; i < imWidth; ++i) {
    for (int j = 0; j < imHeight; ++j) {
    }
  }

  ///TODO: below demonstrates how to use the provided Image class
  ///Should be removed when you start
  Vector3f pixelColor (1.0f,0,0);
  //width and height
  Image image( 10 , 15 );
  image.SetPixel( 5,5, pixelColor );
  image.SaveImage("demo.bmp");
  return 0;
}

