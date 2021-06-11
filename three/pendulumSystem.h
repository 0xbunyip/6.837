#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath/vecmath.h>
#include <vector>
#include <GL/glut.h>

#include "graph.hpp"
#include "particle.hpp"
#include "particleSystem.h"
#include "spring.hpp"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);

	vector<Vector3f> evalF(vector<Vector3f> state);
  void draw();
  ~PendulumSystem(){};

private:
  Graph<std::unique_ptr<Particle>, std::unique_ptr<Spring>> graph_;
};

#endif
