#ifndef PARTICLE_H
#define PARTICLE_H

#include <vecmath/vecmath.h>

constexpr float G_CONSTANT = 9.81;
constexpr Vector3f G_DIR = Vector3f(0, -1, 0);
constexpr Vector3f GRAVITY = G_CONSTANT * G_DIR;

class Particle {
public:
  Particle(const Vector3f& p, const float m): p_(p), m_(m) {}
  Vector3f& p() { return p_; }
  Vector3f& v() { return v_; }

  std::unique_ptr<Particle> Copy(const Vector3f &p, const Vector3f &v);

private:
  Vector3f p_, v_;
  float m_;
};

class FixedParticle : public Particle {
public:
  FixedParticle(const Vector3f& p): p_(p), m_(1) {}

private:
  Vector3f p2_, v2_;
}

class VParticle : public Particle {
  VParticle(const Vector3f& p, const float m, const float kv): Particle(p, m), kv_(kv) {}

private:
  float kv_;
}

#endif
