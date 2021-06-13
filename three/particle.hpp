#ifndef PARTICLE_H
#define PARTICLE_H

#include <vecmath/vecmath.h>
#include <memory>

constexpr float G_CONSTANT = 0.4;
static const Vector3f G_DIR = Vector3f(0, -1, 0);
static const Vector3f GRAVITY = G_CONSTANT * G_DIR;

class Particle {
public:
  Particle(const Vector3f& p, const Vector3f& v, const float m): p_(p), v_(v), m_(m) {}

  float m() { return m_; }

  virtual Vector3f& p();
  virtual Vector3f& v();
  virtual std::unique_ptr<Particle> Copy(const Vector3f &p, const Vector3f &v);
  virtual Vector3f netForce(const Vector3f& externalForce = Vector3f::ZERO);

protected:
  Vector3f p_, v_;
  float m_;
};

class FixedParticle : public Particle {
public:
  FixedParticle(const Vector3f& p, const float m): Particle(p, Vector3f::ZERO, m) {}

  Vector3f& p() override;
  Vector3f& v() override;
  Vector3f netForce(const Vector3f& externalForce = Vector3f::ZERO) override;
  std::unique_ptr<Particle> Copy(const Vector3f &p, const Vector3f &v) override;

private:
  Vector3f p2_, v2_;
};

class VParticle : public Particle {
public:
  VParticle(const Vector3f& p, const Vector3f& v, const float m, const float kv): Particle(p, v, m), kv_(kv) {}

  std::unique_ptr<Particle> Copy(const Vector3f &p, const Vector3f &v) override;
  Vector3f netForce(const Vector3f& externalForce = Vector3f::ZERO) override;

private:
  float kv_;
};

#endif
