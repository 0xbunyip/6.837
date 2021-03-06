#include "ClothSystem.h"
#include "util.h"

#include <GL/glut.h>
#include <cmath>
#include <memory>
#include <random>

constexpr int n = 60;
constexpr int m = 80;
constexpr float dist = 0.05;
constexpr float totalMass = 500.0;
constexpr float kParticleMass = totalMass / n / m;
constexpr float kViscousDrag = 0.1;
const Vector3f kWindForce = Vector3f(0, 0, 0.0);

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(-0.02, 0.02);
std::uniform_real_distribution<double> windDistribution(0.2, 0.4);

int getGridIndex(int i, int j) { return i * m + j; }

// float getDist(int i1, int j1, int i2, int j2) {
//   float x = abs(i1 - i2) * dist;
//   float y = abs(j1 - j2) * dist;
//   return sqrt(x * x + y * y);
// }

ClothSystem::ClothSystem() {
  m_numParticles = n * m;

  auto getDist = [&](int x, int y) -> float {
    return (graph_.v(x)->p() - graph_.v(y)->p()).abs();
  };

  // fill in code for initializing the state based on the number of particles
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (i + j == 0 || (i == 0 && j == m - 1)) {
        std::unique_ptr<Particle> p(
            new FixedParticle(Vector3f(j * dist, -i * dist, 0), kParticleMass));
        graph_.addV(std::move(p));
      } else {
        std::unique_ptr<Particle> p(
            new VParticle(Vector3f(j * dist, distribution(generator), i * dist),
                          Vector3f::ZERO, kParticleMass, kViscousDrag));
        graph_.addV(std::move(p));
      }
    }
  }

  // Structural springs.
  float kSpringConstant = 250.0;
  for (int j = 1; j < m; ++j) {
    std::unique_ptr<Spring> s(new Spring(
        getDist(getGridIndex(0, j), getGridIndex(0, j - 1)), kSpringConstant));
    graph_.addE(std::move(s), j, j - 1);
  }
  for (int i = 1; i < n; ++i) {
    std::unique_ptr<Spring> s(new Spring(
        getDist(getGridIndex(i, 0), getGridIndex(i - 1, 0)), kSpringConstant));
    graph_.addE(std::move(s), i * m, (i - 1) * m);
  }
  for (int i = 1; i < n; ++i) {
    for (int j = 1; j < m; ++j) {
      // (i, j) to (i - 1, j)
      int x = getGridIndex(i, j);
      int y = getGridIndex(i - 1, j);
      std::unique_ptr<Spring> s(new Spring(getDist(x, y), kSpringConstant));
      graph_.addE(std::move(s), x, y);

      // (i, j) to (i, j - 1)
      y = getGridIndex(i, j - 1);
      std::unique_ptr<Spring> s2(new Spring(getDist(x, y), kSpringConstant));
      graph_.addE(std::move(s2), x, y);
    }
  }

  // Shear springs.
  kSpringConstant = 30.0;
  for (int i = 1; i < n; ++i) {
    for (int j = 1; j < m; ++j) {
      // (i, j) to (i - 1, j - 1)
      int x = getGridIndex(i, j);
      int y = getGridIndex(i - 1, j - 1);
      std::unique_ptr<Spring> s(
          new Spring(getDist(x, y), kSpringConstant));
      graph_.addE(std::move(s), x, y);

      // (i, j - 1) to (i - 1, j)
      x = getGridIndex(i, j - 1);
      y = getGridIndex(i - 1, j);
      std::unique_ptr<Spring> s2(new Spring(getDist(x, y), kSpringConstant));
      graph_.addE(std::move(s2), x, y);
    }
  }

  // Flex springs.
  kSpringConstant = 30.0;
  for (int i = 0; i + 2 < n; ++i) {
    for (int j = 0; j < m; ++j) {
      // (i, j) to (i + 2, j)
      int x = getGridIndex(i, j);
      int y = getGridIndex(i + 2, j);
      std::unique_ptr<Spring> s(new Spring(getDist(x, y), kSpringConstant));
      graph_.addE(std::move(s), x, y);
    }
  }
  for (int j = 0; j + 2 < m; ++j) {
    for (int i = 0; i < n; ++i) {
      // (i, j) to (i, j + 2)
      int x = getGridIndex(i, j);
      int y = getGridIndex(i, j + 2);
      std::unique_ptr<Spring> s(new Spring(getDist(x, y), kSpringConstant));
      graph_.addE(std::move(s), x, y);
    }
  }

  for (int i = 0; i < m_numParticles; i++) {
    // for this system, we care about the position and the velocity
    m_vVecState.push_back(graph_.v(i)->p());
    m_vVecState.push_back(graph_.v(i)->v());
  }
}

// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
  vector<Vector3f> f;
  Vector3f wind = kWindForce * windDistribution(generator);
  for (int i = 0; i < m_numParticles; ++i) {
    std::unique_ptr<Particle> part =
        graph_.v(i)->Copy(state[i * 2], state[i * 2 + 1]);

    Vector3f externalForce(0, 0, 0);
    externalForce += wind;
    for (const auto &adj : graph_.adj(i)) {
      int eid = adj.eid;
      int j = adj.v;
      externalForce =
          externalForce + graph_.e(eid)->Force(state[i * 2], state[j * 2]);
    }

    auto v = state[i * 2 + 1];
    auto a = part->netForce(externalForce) / part->m();

    if (i == getGridIndex(1, 0)) {
      // LOG(v, a, externalForce);
    }

    // LOG(i, v, a, part->netForce(externalForce));
    f.push_back(v);
    f.push_back(a);
  }
	return f;
}

void ClothSystem::draw() {
  // for (int i = 0; i < m_numParticles; i++) {
  //   Vector3f pos(m_vVecState[i * 2]); //  position of particle i.
  //   glPushMatrix();
  //   glTranslatef(pos[0], pos[1], pos[2]);
  //   glutSolidSphere(0.075f, 10.0f, 10.0f);
  //   glPopMatrix();

  //   // for (const auto &adj : graph_.adj(i)) {
  //   //   int j = adj.v;
  //   //   Vector3f xi = m_vVecState[i * 2];
  //   //   Vector3f xj = m_vVecState[j * 2];

  //   //   glPushMatrix();
  //   //   // glTranslatef(xi[0], xi[1], xi[2]);

  //   //   // Save current state of OpenGL
  //   //   glPushAttrib(GL_ALL_ATTRIB_BITS);

  //   //   glDisable(GL_LIGHTING);
  //   //   glLineWidth(3);
  //   //   glPushMatrix();
  //   //   glBegin(GL_LINES);
  //   //   glColor4f(0.5, 0.5, 0.5, 1);
  //   //   glVertex3f(xi[0], xi[1], xi[2]);
  //   //   glVertex3f(xj[0], xj[1], xj[2]);
  //   //   glEnd();
  //   //   glPopMatrix();

  //   //   glPopAttrib();
  //   //   glPopMatrix();
  //   // }
  // }

  // Calculate per-vertex normal.
  std::vector<int> counts(m_numParticles);
  std::vector<Vector3f> normals(m_numParticles);
  auto updateNormal = [&](int i, const Vector3f norm) {
    counts[i]++;
    normals[i] += norm;
  };

  for (int i = 0; i + 1 < n; ++i) {
    for (int j = 0; j + 1 < m; ++j) {
      int ai = getGridIndex(i, j);
      int bi = getGridIndex(i, j + 1);
      int ci = getGridIndex(i + 1, j);
      int di = getGridIndex(i + 1, j + 1);
      Vector3f a = m_vVecState[ai * 2];
      Vector3f b = m_vVecState[bi * 2];
      Vector3f c = m_vVecState[ci * 2];
      Vector3f d = m_vVecState[di * 2];
      // LOG(a, b, c, d);

      // adb
      auto norm = Vector3f::cross(d - a, b - a).normalized();
      updateNormal(ai, norm);
      updateNormal(bi, norm);
      updateNormal(di, norm);

      // adc
      norm = Vector3f::cross(c - a, d - a).normalized();
      updateNormal(ai, norm);
      updateNormal(ci, norm);
      updateNormal(di, norm);
    }
  }
  for (int i = 0; i < m_numParticles; ++i) {
    normals[i] = (normals[i] / counts[i]).normalized();
    // LOG(i / m, i % m, counts[i]);
  }

  GLuint index = glGenLists(1);
  // compile the display list
  glNewList(index, GL_COMPILE);

  glBegin(GL_TRIANGLES);
  for (int i = 0; i + 1 < n; ++i) {
    for (int j = 0; j + 1 < m; ++j) {
      int ai = getGridIndex(i, j);
      int bi = getGridIndex(i, j + 1);
      int ci = getGridIndex(i + 1, j);
      int di = getGridIndex(i + 1, j + 1);
      Vector3f a = m_vVecState[ai * 2];
      Vector3f b = m_vVecState[bi * 2];
      Vector3f c = m_vVecState[ci * 2];
      Vector3f d = m_vVecState[di * 2];
      // LOG(a, b, c, d);

      // adb
      glVertex3d(a[0], a[1], a[2]);
      glNormal3fv(normals[ai]);
      glVertex3d(d[0], d[1], d[2]);
      glNormal3fv(normals[di]);
      glVertex3d(b[0], b[1], b[2]);
      glNormal3fv(normals[bi]);
      // auto norm = Vector3f::cross(d - a, b - a).normalized();
      // LOG(i, j, norm);
      // glNormal3d(norm[0], norm[1], norm[2]);

      // adc
      glVertex3d(a[0], a[1], a[2]);
      glNormal3fv(normals[ai]);
      glVertex3d(c[0], c[1], c[2]);
      glNormal3fv(normals[ci]);
      glVertex3d(d[0], d[1], d[2]);
      glNormal3fv(normals[di]);
      // norm = Vector3f::cross(c - a, d - a).normalized();
      // glNormal3d(norm[0], norm[1], norm[2]);
    }
  }
  glEnd();

  // End list.
  glEndList();

  glCallList(index);
}

