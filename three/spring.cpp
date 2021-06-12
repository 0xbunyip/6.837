#include "spring.hpp"

Vector3f Spring::Force(const Vector3f &xi, const Vector3f &xj) {
  auto d = xi - xj;
  auto l = d.abs();
  return d * (1.0 / l) * -k_ * (l - r_);
}
