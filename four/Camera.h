#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

#include <cmath>
#include <float.h>
#include <vecmath/vecmath.h>

#include "util.h"

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
    direction = direction_.normalized();
    up = up_.normalized();
    angle = angle_;
    right = Vector3f::cross(direction, up).normalized();
    d = 1.0 / tan(angle * 0.5);
  }

  virtual Ray generateRay(const Vector2f &point) {
    auto p = center + (point.x() * right) + (point.y() * up) + (d * direction);
    auto rayDir = p - center;
    rayDir.normalize();
    return Ray(center, rayDir);
  }

  virtual float getTMin() const { return 0.0f; }

private:
  float angle;
  float d;
  Vector3f right;
};

#endif //CAMERA_H
