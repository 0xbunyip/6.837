#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath/vecmath.h>
#include <cmath>
#include <iostream>

#include "util.h"

using namespace std;
class Triangle : public Object3D {
public:
  Triangle(); ///@param a b c are three vertex positions of the triangle

  Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m)
      : Object3D(m), a_(a), b_(b), c_(c) {
    hasTex = false;
    ab_ = b - a;
    ac_ = c - a;
  }

  virtual bool intersect(const Ray &ray, Hit &h, float tmin) {
    auto rdir = ray.getDirection();
    auto M = Matrix3f(-ab_.x(), -ac_.x(), rdir.x(), //
                      -ab_.y(), -ac_.y(), rdir.y(), //
                      -ab_.z(), -ac_.z(), rdir.z());

    if (abs(M.determinant()) < 1e-6) {
      return false;
    }

    auto ro = ray.getOrigin();
    auto y = a_ - ro;
    auto x = M.inverse() * y;
    float beta = x[0], gamma = x[1], t = x[2];

    if (t < tmin) {
      return false;
    }

    if (beta < 0.0 || beta > 1.0 || gamma < 0.0 || gamma > 1.0 ||
        (beta + gamma > 1.0)) {
      return false;
    }

    if (t > h.getT()) {
      return true;
    }

    auto alpha = 1.0f - beta - gamma;
    auto n = alpha * a_ + beta * b_ + gamma * c_;
    n.normalize();
    h.set(t, material, n);

    auto texCoord =
        alpha * texCoords[0] + beta * texCoords[1] + gamma * texCoords[2];
    h.setTexCoord(texCoord);
    return true;
  }

  bool hasTex;
  Vector3f normals[3];
  Vector2f texCoords[3];

protected:
  Vector3f a_, b_, c_, ab_, ac_;
};

#endif //TRIANGLE_H
