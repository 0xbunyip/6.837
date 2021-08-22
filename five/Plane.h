#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath/vecmath.h>
#include <cmath>
using namespace std;

class Plane : public Object3D {
public:
  Plane() {}
  Plane(const Vector3f &normal, float d, Material *m)
      : Object3D(m), normal_(normal.normalized()), d_(d) {}
  ~Plane() {}

  virtual bool intersect(const Ray &r, Hit &h, float tmin) {
    float de = Vector3f::dot(normal_, r.getDirection());
    if (abs(de) < 1e-6) {
      return false;
    }

    float t = (d_ - Vector3f::dot(normal_, r.getOrigin())) / de;
    if (t < tmin) {
      return false;
    }

    if (t > h.getT()) {
      return false;
    }

    h.set(t, material, normal_);
    return true;
  }

protected:
  Vector3f normal_;
  float d_;
};
#endif //PLANE_H

