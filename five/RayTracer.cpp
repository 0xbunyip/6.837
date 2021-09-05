#include "RayTracer.h"
#include "Camera.h"
#include "Group.h"
#include "Hit.h"
#include "Light.h"
#include "Material.h"
#include "Ray.h"
#include "util.h"

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming) {}

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                          float index_n, float index_nt,
                          Vector3f &transmitted) {}

RayTracer::RayTracer(SceneParser *scene, int max_bounces) : m_scene(scene) {
  m_group = scene->getGroup();
  m_maxBounces = max_bounces;
}

RayTracer::~RayTracer() { }

// Returns true if the shadow ray intersects any objects.
bool RayTracer::castShadowRay(const Ray &ray) const {
  Hit hit;
  return m_group->intersectAny(ray, hit, EPSILON);
}

Vector3f RayTracer::traceRay(Ray &ray, float tmin, int bounces,
                             float refr_index, Hit &hit) const {
  if (!m_group->intersect(ray, hit, tmin)) {
    return m_scene->getBackgroundColor(ray.getDirection());
  }

  auto p = ray.pointAtParameter(hit.getT());
  auto material = hit.getMaterial();
  Vector3f color(m_scene->getAmbientLight());

  for (int k = 0; k < m_scene->getNumLights(); ++k) {
    auto light = m_scene->getLight(k);
    float distanceToLight = 0;

    Vector3f lightPos;
    if (light->getPosition(lightPos)) {
      Ray rayToLight(p, (lightPos - p).normalized());
      if (castShadowRay(rayToLight)) {
        continue;
      }
    }

    Vector3f dirToLight, lightColor;
    light->getIllumination(p, dirToLight, lightColor, distanceToLight);

    color += material->Shade(ray, hit, dirToLight, lightColor);
  }
  return color;
}
