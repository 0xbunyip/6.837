#include "RayTracer.h"
#include "Camera.h"
#include "Group.h"
#include "Hit.h"
#include "Light.h"
#include "Material.h"
#include "Ray.h"
#include "util.h"

#define EPSILON 0.001

#define SQR(x) ((x) * (x))

//IMPLEMENT THESE FUNCTIONS
Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming) {
  return 2 * Vector3f::dot(-incoming, normal) * normal + incoming;
}

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming,
                          float index_n, float index_nt,
                          Vector3f &transmitted) {
  float dN = Vector3f::dot(incoming, normal);
  float c = 1 - SQR(index_n) * (1 - SQR(dN)) / SQR(index_nt);
  if (c < 0) {
    return false;
  }
  auto a = index_n * (incoming - normal * dN) / index_nt;
  auto b = normal * sqrt(c);
  transmitted = a - b;
  return true;
}

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

float reflectionWeight(const Vector3f &normal, const Vector3f &incoming,
                       const Vector3f &transmitted, float index_n,
                       float index_nt) {
  float R0 = SQR((index_nt - index_n) / (index_nt + index_n));
  float c = index_n < index_nt ? abs(Vector3f::dot(incoming, normal))
                               : abs(Vector3f::dot(transmitted, normal));
  return R0 + (1 - R0) * pow((1 - c), 5);
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
  Hit reflHit;
  auto reflectionColor =
      material->getSpecularColor() *
      traceRay(reflectionRay, EPSILON, bounces - 1, refr_index, reflHit);

  // Trace refraction rays.
  float mrefr_index = material->getRefractionIndex();
  float R = 1.0;
  Vector3f refractionColor(0, 0, 0);
  if (mrefr_index > 0) {
    Vector3f refractionDir;
    if (transmittedDirection(hit.getNormal(), ray.getDirection(), refr_index,
                             mrefr_index, refractionDir)) {
      Ray refractionRay(p, refractionDir);
      Hit refrHit;
      refractionColor =
          traceRay(refractionRay, EPSILON, bounces - 1, mrefr_index, refrHit);
      R = reflectionWeight(hit.getNormal(), ray.getDirection(), refractionDir,
                           refr_index, mrefr_index);
    }
  }
  color += R * reflectionColor + (1 - R) * refractionColor;

  return color;
}
