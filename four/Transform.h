#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Object3D.h"
#include "util.h"
#include <vecmath/vecmath.h>

class Transform: public Object3D {
public:
  Transform(){}
  Transform(const Matrix4f &m, Object3D *obj) : o(obj), m_(m) {
    mi = m_.inverse();
  }
  ~Transform(){ }

  virtual bool intersect(const Ray &r, Hit &h, float tmin) {
    Vector4f originInObjectSpace = mi * Vector4f(r.getOrigin(), 1.0);
    Vector4f dirInObjectSpace = mi * Vector4f(r.getDirection(), 0.0);
    Ray r2(originInObjectSpace.xyz(), dirInObjectSpace.xyz());

    bool intersected = o->intersect(r2, h, tmin);

    const Vector4f normalInWorldSpace =
        mi.transposed() * Vector4f(h.getNormal(), 0.0);
    h.set(h.getT(), h.getMaterial(), normalInWorldSpace.xyz());

    return intersected;
  }

protected:
  Object3D *o; // un-transformed object
  Matrix4f m_, mi;
};

#endif //TRANSFORM_H
