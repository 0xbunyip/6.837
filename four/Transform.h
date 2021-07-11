#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath/vecmath.h>
#include "Object3D.h"

class Transform: public Object3D {
public:
  Transform(){}
  Transform(const Matrix4f &m, Object3D *obj) : o(obj) {}
  ~Transform(){ }

  virtual bool intersect(const Ray &r, Hit &h, float tmin) {
    o->intersect(r, h, tmin);
  }

protected:
  Object3D *o; // un-transformed object
};

#endif //TRANSFORM_H
