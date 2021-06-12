#include "TimeStepper.hpp"
#include "util.h"

void ForwardEuler::takeStep(ParticleSystem *particleSystem, float stepSize) {
  auto state = particleSystem->getState();
  const auto dx = particleSystem->evalF(state);
  for (int i = 0; i < dx.size(); ++i) {
    state[i] = state[i] + stepSize * dx[i];
    LOG(i, state[i]);
  }
  particleSystem->setState(state);
}

void Trapzoidal::takeStep(ParticleSystem *particleSystem, float stepSize) {
  auto state = particleSystem->getState();
  const auto dx1 = particleSystem->evalF(state);

  auto state2 = state;
  for (int i = 0; i < dx1.size(); ++i) {
    state2[i] = state2[i] + stepSize * dx1[i];
  }
  const auto dx2 = particleSystem->evalF(state2);

  for (int i = 0; i < dx1.size(); ++i) {
    const auto dx = 0.5 * dx1[i] + 0.5 * dx2[i];
    state[i] = state[i] + stepSize * dx;
  }
  particleSystem->setState(state);
}
