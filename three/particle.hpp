#ifndef PARTICLE_H
#define PARTICLE_H

#include <vecmath/vecmath.h>
#include <memory>

constexpr float G_CONSTANT = 9.81;
// constexpr Vector3f G_DIR = Vector3f(0, -1, 0);
// constexpr Vector3f GRAVITY = G_CONSTANT * G_DIR;

class Particle {
public:
  Particle(const Vector3f& p, const Vector3f& v, const float m): p_(p), v_(v), m_(m) {}

  virtual Vector3f& p();
  virtual Vector3f& v();
  virtual std::unique_ptr<Particle> Copy(const Vector3f &p, const Vector3f &v);

protected:
  Vector3f p_, v_;
  float m_;
};

class FixedParticle : public Particle {
public:
  FixedParticle(const Vector3f& p): Particle(p, Vector3f::ZERO, 1) {}

  Vector3f& p() override;
  Vector3f& v() override;
  std::unique_ptr<Particle> Copy(const Vector3f &p, const Vector3f &v) override;

private:
  Vector3f p2_, v2_;
};

class VParticle : public Particle {
  VParticle(const Vector3f& p, const Vector3f& v, const float m, const float kv): Particle(p, v, m), kv_(kv) {}

  std::unique_ptr<Particle> Copy(const Vector3f &p, const Vector3f &v) override;

private:
  float kv_;
}

#endif
