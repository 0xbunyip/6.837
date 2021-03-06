#include "pendulumSystem.h"
#include "util.h"

#include <memory>

PendulumSystem::PendulumSystem(int numParticles)
    : ParticleSystem(numParticles) {
  numParticles = 4;
  m_numParticles = numParticles;

  // fill in code for initializing the state based on the number of particles
  const float kViscousDrag = 0.01;
  std::unique_ptr<Particle> p0(new FixedParticle(Vector3f(0, 0, 0), 1.0));
  std::unique_ptr<Particle> p1(
      new VParticle(Vector3f(-1, -1, 0), Vector3f::ZERO, 1.0, kViscousDrag));
  std::unique_ptr<Particle> p2(
      new VParticle(Vector3f(1, -2, 0), Vector3f::ZERO, 1.0, kViscousDrag));
  std::unique_ptr<Particle> p3(
      new VParticle(Vector3f(3, -2, 0), Vector3f::ZERO, 1.0, kViscousDrag));

  int i0 = graph_.addV(std::move(p0));
  int i1 = graph_.addV(std::move(p1));
  int i2 = graph_.addV(std::move(p2));
  int i3 = graph_.addV(std::move(p3));
  std::unique_ptr<Spring> s(new Spring(1, 0.6));
  graph_.addE(std::move(s), i0, i1);

  std::unique_ptr<Spring> s2(new Spring(1, 0.5));
  graph_.addE(std::move(s2), i1, i2);

  std::unique_ptr<Spring> s3(new Spring(1, 0.5));
  graph_.addE(std::move(s3), i1, i3);

  std::unique_ptr<Spring> s4(new Spring(1, 0.4));
  graph_.addE(std::move(s4), i2, i3);

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

    for (const auto &adj : graph_.adj(i)) {
      int j = adj.v;
      Vector3f xi = m_vVecState[i * 2];
      Vector3f xj = m_vVecState[j * 2];

      glPushMatrix();
      // glTranslatef(xi[0], xi[1], xi[2]);

      // Save current state of OpenGL
      glPushAttrib(GL_ALL_ATTRIB_BITS);

      // This is to draw the axes when the mouse button is down
      glDisable(GL_LIGHTING);
      glLineWidth(3);
      glPushMatrix();
      glBegin(GL_LINES);
      glColor4f(0.5, 0.5, 0.5, 1);
      glVertex3f(xi[0], xi[1], xi[2]);
      glVertex3f(xj[0], xj[1], xj[2]);
      glEnd();
      glPopMatrix();

      glPopAttrib();
      glPopMatrix();
    }
  }
}
