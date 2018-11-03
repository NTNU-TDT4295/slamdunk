#include "Object.h"
#include "linmath.h"

Object::Object() noexcept
{}

Object::~Object()
{}

float Object::GetIntersection(const Ray &)
{
  return 0.0;
}

vec3 Object::GetNormalAt(const vec3 &)
{
  return vec3(0.0, 0.0, 0.0);
}

vec3 Object::GetTexCoords(vec3 &, const vec3 &)
{
  return vec3(0.0, 0.0, 0.0);
}
