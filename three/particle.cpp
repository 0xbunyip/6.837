#include "particle.hpp"

#include <memory>
#include <vecmath/vecmath.h>

// Vector3f& Particle::p() { return p_; }
// Vector3f& Particle::v() { return v_; }

std::unique_ptr<Particle> Particle::Copy(const Vector3f &p, const Vector3f &v) {
  std::unique_ptr<Particle> part(new Particle(p, v, m_));
  return part;
}

Vector3f& FixedParticle::p() {
  p2_ = p_;
  return p2_;
}

Vector3f &FixedParticle::v() {
  v2_ = Vector3f(0, 0, 0);
  return v2_;
}

std::unique_ptr<Particle> FixedParticle::Copy(const Vector3f &p,
                                              const Vector3f &v) {
  std::unique_ptr<Particle> part(new FixedParticle(p));
  return part;
}

std::unique_ptr<Particle> VParticle::Copy(const Vector3f &p,
                                          const Vector3f &v) {
  std::unique_ptr<Particle> part(new VParticle(p, v, m_, kv_));
  return part;
}

