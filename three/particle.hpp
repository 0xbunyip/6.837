#ifndef PARTICLE_H
#define PARTICLE_H

#include <vecmath/vecmath.h>

class Particle {
public:
  Particle(const Vector3f& p): p_(p) {}
  Vector3f& p() { return p_; }
  Vector3f& v() { return v_; }

private:
  Vector3f p_, v_;
};

class FixedParticle : public Particle {
public:
  FixedParticle(const Vector3f& p): p_(p) {}

  Vector3f& p() {
    p2_ = p;
    return p2_;
  }

  Vector3f& v() {
    v2_ = v;
    return v2_;
  }

private:
  Vector3f p2_, v2_;
}

constexpr float G_CONSTANT = 9.81;
constexpr Vector3f G_DIR = Vector3f(0, -1, 0);

class GParticle : public Particle {
public:
  GParticle(const Vector3f& p, const float m): p_(p), m_(m) {}
  static Vector3f GRAVITY = G_CONSTANT * G_DIR;

private:
  float m_;
}

class GVParticle : public GParticle {
  GVParticle(const Vector3f& p, const float m, const float vk): GParticle(p, m), vk_(vk) {}

private:
  float vk_;
}

#endif
