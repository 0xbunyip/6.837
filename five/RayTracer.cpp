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
Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming) {
  return 2 * Vector3f::dot(-incoming, normal) * normal + incoming;
}

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
  auto background = m_scene->getBackgroundColor(ray.getDirection());
  if (bounces < 0) {
    return background;
  }

  if (!m_group->intersect(ray, hit, tmin)) {
    return background;
  }

  auto p = ray.pointAtParameter(hit.getT());
  auto material = hit.getMaterial();
  // LOG(hit.getT(), p);
  Vector3f color(m_scene->getAmbientLight() * material->getDiffuseColorAt(hit));

  for (int k = 0; k < m_scene->getNumLights(); ++k) {
    auto light = m_scene->getLight(k);
    float distanceToLight = 0;

    // Cast shadow ray.
    Ray rayToLight(p, light->getDirectionToLight(p));
    if (castShadowRay(rayToLight)) {
      continue;
    }

    Vector3f dirToLight, lightColor;
    light->getIllumination(p, dirToLight, lightColor, distanceToLight);

    // LOG(ray, hit, dirToLight, lightColor);
    color += material->Shade(ray, hit, dirToLight, lightColor);
  }

  // Trace reflection rays.
  Ray reflectionRay(p, mirrorDirection(hit.getNormal(), ray.getDirection()));
  Hit refHit;
  color += material->getSpecularColor() *
           traceRay(reflectionRay, EPSILON, bounces - 1, refr_index, refHit);

  return color;
}
