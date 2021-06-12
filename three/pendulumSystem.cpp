#include "pendulumSystem.h"
#include "util.h"

#include <memory>

PendulumSystem::PendulumSystem(int numParticles)
    : ParticleSystem(numParticles) {
  LOG(numParticles);
  m_numParticles = numParticles;

  // fill in code for initializing the state based on the number of particles
  const float kViscousDrag = 0.0;
  std::unique_ptr<Particle> p0(new FixedParticle(Vector3f(0, -1, 0)));
  std::unique_ptr<Particle> p1(
      new VParticle(Vector3f(0, -2, 0), Vector3f::ZERO, 1.0, kViscousDrag));

  int i0 = graph_.addV(std::move(p0));
  int i1 = graph_.addV(std::move(p1));
  std::unique_ptr<Spring> s(new Spring(1, 0.2));
  graph_.addE(std::move(s), i0, i1);

  for (int i = 0; i < m_numParticles; i++) {
    // for this system, we care about the position and the velocity
    m_vVecState.push_back(graph_.v(i)->p());
    m_vVecState.push_back(graph_.v(i)->v());
  }
}

// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state) {
  vector<Vector3f> f;
  for (int i = 0; i < m_numParticles; ++i) {
    std::unique_ptr<Particle> part =
        graph_.v(i)->Copy(state[i * 2], state[i * 2 + 1]);

    Vector3f externalForce(0, 0, 0);
    for (const auto &adj : graph_.adj(i)) {
      int eid = adj.eid;
      int j = adj.v;
      externalForce =
          externalForce + graph_.e(eid)->Force(state[i * 2], state[j * 2]);
    }

    auto v = state[i * 2 + 1];
    auto a = part->netForce(externalForce) / part->m();

    LOG(i, v, a, part->netForce(externalForce));
    f.push_back(v);
    f.push_back(a);
  }
	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw() {
  for (int i = 0; i < m_numParticles; i++) {
    Vector3f pos(m_vVecState[i * 2]); //  position of particle i. YOUR CODE HERE
    LOG(i, pos);
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glutSolidSphere(0.075f, 10.0f, 10.0f);
    glPopMatrix();
  }
}
