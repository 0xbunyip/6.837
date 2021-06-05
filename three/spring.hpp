#ifndef SPRING_H
#define SPRING_H

// class Force {
//   Force(const Vector3f& v): v_(v) {};
//
//   float norm() {
//     return v_.abs();
//   }
//
//   Vector3f dir() {
//     return v_.normalized();
//   }
//
// public:
//   Vector3f v_;
// };
//
// static Gravity GRAVITY = Force(Vector3f(0, -9.81, 0));
//
// class ViscousDrag : public Force {
//   ViscousDrag(const float k): k_(k) {};
//
// public:
//   float k_;
// };

class Spring {
  Spring(const float r, const float k): r_(r), k_(k) {}

public:
  float r_, k_;
};

#endif
