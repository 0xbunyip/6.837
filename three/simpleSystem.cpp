
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem() { m_vVecState.emplace_back(1, 0, 0); }

// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state) {
	vector<Vector3f> f;
  for (const auto& s : state) {
    f.emplace_back(-s.y(), s.x(), 0);
  }
	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw() {
  auto pos = m_vVecState[0];
  glPushMatrix();
  glTranslatef(pos[0], pos[1], pos[2]);
  glutSolidSphere(0.075f, 10.0f, 10.0f);
  glPopMatrix();
}
