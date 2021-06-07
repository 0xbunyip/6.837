#include "particle.hpp"

Vector3f &Particle::p() { return p_; }
Vector3f &Particle::v() { return v_; }

std::unique_ptr<Particle> Particle::Copy(const Vector3f &p, const Vector3f &v) {
  std::unique_ptr<Particle> part = new Particle(p, m_);
  part.v_ = v;
  return part;
}

Vector3f &FixedParticle::p() {
  p2_ = p;
  return p2_;
}

Vector3f &FixedParticle::v() {
  v2_ = Vector3f(0, 0, 0);
  return v2_;
}

std::unique_ptr<Particle> FixedParticle::Copy(const Vector3f &p,
                                              const Vector3f &v) {
  Particle part;
  part.p_ = p;
  part.v_ = v;
  part.m_ = m_;
  part.kv_ = kv_;
  return part;
}

