#ifndef RAY_H
#define RAY_H

#include <cassert>
#include <iostream>
#include <vecmath/vecmath.h>

using namespace std;

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray
{
public:
  Ray(const Vector3f &orig, const Vector3f &dir) {
    origin = orig;
    direction = dir;
  }

  Ray(const Ray &r) {
    origin = r.origin;
    direction = r.direction;
  }

  const Vector3f &getOrigin() const { return origin; }

  const Vector3f &getDirection() const { return direction; }

  Vector3f pointAtParameter(float t) const { return origin + direction * t; }

private:
    // don't use this constructor
  Ray() { assert(false); }

  Vector3f origin;
  Vector3f direction;

};

inline ostream &operator<<(ostream &os, const Ray &r) {
  os << "<o:" << r.getOrigin() << ", d:" << r.getDirection() << ">";
  return os;
}

#endif // RAY_H
