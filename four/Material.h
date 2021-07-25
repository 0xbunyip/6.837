#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath/vecmath.h>

#include "Hit.h"
#include "Ray.h"
#include "texture.hpp"
#include "util.h"

class Material {
public:
  Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO,
           float s = 0)
      : diffuseColor(d_color), specularColor(s_color), shininess(s) {
        LOG(specularColor);
        LOG(diffuseColor);
        LOG(shininess);
      }

  virtual ~Material() {}

  virtual Vector3f getDiffuseColor() const { return diffuseColor; }

  Vector3f Shade(const Ray &ray, const Hit &hit, const Vector3f &dirToLight,
                 const Vector3f &lightColor) {
    auto diffuseWeight = Vector3f::dot(dirToLight, hit.getNormal());
    diffuseWeight = max(diffuseWeight, 0.0f);

    auto materialColor = diffuseColor;
    if (t.valid()) {
      materialColor = t(hit.texCoord.x(), hit.texCoord.y());
    }

    auto reflectionRay =
        2 * Vector3f::dot(dirToLight, hit.getNormal()) * hit.getNormal() -
        dirToLight;
    auto specularWeight = Vector3f::dot(dirToLight, reflectionRay);
    specularWeight = pow(max(specularWeight, 0.0f), shininess);

    return diffuseWeight * lightColor * materialColor +
           specularWeight * lightColor * specularColor;
  }

 void loadTexture(const char *filename) { t.load(filename); }

 protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  float shininess;
  Texture t;
};

#endif // MATERIAL_H
