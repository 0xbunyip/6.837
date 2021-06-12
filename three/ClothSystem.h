#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath/vecmath.h>
#include <vector>

#include "graph.hpp"
#include "particle.hpp"
#include "particleSystem.h"
#include "spring.hpp"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem();
	vector<Vector3f> evalF(vector<Vector3f> state);
  void draw();

private:
  Graph<std::unique_ptr<Particle>, std::unique_ptr<Spring>> graph_;
};

#endif
