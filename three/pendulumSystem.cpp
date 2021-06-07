#include "pendulumSystem.h"
#include "graph.hpp"

using std::unique_ptr;

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles) {
	m_numParticles = numParticles;

	// fill in code for initializing the state based on the number of particles
  unique_ptr<Particle> p0 = new FixedParticle(Vector3f(0, 0, 0));
  Particle p1 = new VParticle(Vector3f(0, -2, 0), 1, 0.1);

  Graph<unique_ptr<Particle>, unique_ptr<Spring>> graph;
  int i0 = graph.addV(p0);
  int i1 = graph.addV(p1);
  graph.addE(Spring(1, 0.2), i0, i1);

  for (int i = 0; i < m_numParticles; i++) {
    // for this system, we care about the position and the velocity
    m_vVecState.push_back(graph.v(i).p());
    m_vVecState.push_back(graph.v(i).v());
  }
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state) {
	vector<Vector3f> f;
  for (int i = 0; i < m_numParticles; ++i) {
    unique_ptr<Particle> part =
        graph.v(i)->Copy(state[i * 2], state[i * 2 + 1]);

    f.push_back(state[i * 2 + 1]);
    f.push_back(part->F() / part->m());
  }
	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw() {
  for (int i = 0; i < m_numParticles; i++) {
    Vector3f pos; //  position of particle i. YOUR CODE HERE
    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glutSolidSphere(0.075f, 10.0f, 10.0f);
    glPopMatrix();
  }
}
