#ifndef GROUP_H
#define GROUP_H

#include "Hit.h"
#include "Object3D.h"
#include "Ray.h"
#include "util.h"

#include <iostream>
#include <memory>

using namespace std;

class Group : public Object3D {
public:
  Group() {}

  Group(int num_objects) {
    objects_ = vector<unique_ptr<Object3D>>(num_objects);
  }

  ~Group() {}

  virtual bool intersect(const Ray &r, Hit &h, float tmin) {
    bool intersected = false;
    for (const auto& obj : objects_) {
      intersected |= obj->intersect(r, h, tmin);
    }
    return intersected;
  }

  void addObject(int index, Object3D *obj) {
    objects_[index].reset(obj); //
  }

  int getGroupSize() { return objects_.size(); }

private:
  vector<unique_ptr<Object3D>> objects_;
};

#endif

