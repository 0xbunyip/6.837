#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <cmath>
#include <vecmath/vecmath.h>

#include <iostream>
using namespace std;

class Sphere : public Object3D {
public:
  Sphere() {
    // unit ball at the center
  }

  Sphere(Vector3f center, float radius, Material *material)
      : Object3D(material), center_(center), radius_(radius) {}

  ~Sphere() { delete material; }

  virtual bool intersect(const Ray &r, Hit &h, float tmin) {
    auto Ro = r.getOrigin();
    auto Rd = r.getDirection();
    auto b = Ro - center_;

    float A = Rd.absSquared();
    float B = 2 * Vector3f::dot(Rd, b);
    float C = b.absSquared() - radius_ * radius_;
    float delta = B * B - 4 * A * C;

    if (delta < 0) {
      return false;
    }

    auto t1 = (-B - sqrt(delta)) / A / 2;
    auto t2 = (-B + sqrt(delta)) / A / 2;
    if (t2 < tmin) {
      return false;
    }

    auto t = t1 >= tmin ? t1 : t2;
    if (t > h.getT()) {
      return true;
    }

    auto P = Ro + t * Rd;
    auto n = (P - center_).normalized();
    h.set(t, material, n);
    return true;
  }

protected:
  Vector3f center_;
  float radius_;
};

#endif
