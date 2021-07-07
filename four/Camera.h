#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

#include <cmath>
#include <float.h>
#include <vecmath/vecmath.h>

class Camera {
public:
  // generate rays for each screen-space coordinate
  virtual Ray generateRay(const Vector2f &point) = 0;
  virtual float getTMin() const = 0;
  virtual ~Camera() {}

protected:
	Vector3f center;
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;
};

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(const Vector3f &center_, const Vector3f &direction_,
                    const Vector3f &up_, float angle_) {
    center = center_;
    direction = direction_;
    up = up_;
    angle = angle_;
    right = Vector3f::cross(direction, up);
  }

  virtual Ray generateRay(const Vector2f &point) {
    auto p = (-point.x() * right) + (-point.y() * up);
    return Ray(center, p - center);
  }

  virtual float getTMin() const { return 0.0f; }

private:
  float angle;
  Vector3f right;
};

#endif //CAMERA_H
