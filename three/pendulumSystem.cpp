#include "pendulumSystem.h"
#include "graph.hpp"

using std::unique_ptr;

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles) {
	m_numParticles = numParticles;

	// fill in code for initializing the state based on the number of particles
  Particle p0 = new FixedParticle();
  Particle p1 = new GParticle();
  Particle p1 = new GVParticle();
  Particle p1 = new GVPParticle();

  Graph<unique_ptr<Particle>, unique_ptr<Spring>> graph;

  for (int i = 0; i < m_numParticles; i++) {
    // for this system, we care about the position and the velocity
    m_vVecState.push_back();
    m_vVecState.push_back();
  }
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state) {
	vector<Vector3f> f;

	// YOUR CODE HERE

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
