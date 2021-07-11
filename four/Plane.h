#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath/vecmath.h>
#include <cmath>
using namespace std;

class Plane : public Object3D {
public:
  Plane() {}
  Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {}
  ~Plane() {}

  virtual bool intersect(const Ray &r, Hit &h, float tmin) { return false; }

protected:
  Vector3f normal_;
  float d;
};
#endif //PLANE_H

