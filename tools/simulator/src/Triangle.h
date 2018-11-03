#pragma once

#include "linmath.h"
#include <glm/glm.hpp>
#include "Object.h"
#include "Ray.h"

class Triangle: public Object
{
public:
	Triangle();
	Triangle(vec3 &v0_, vec3 &v1_, vec3 &v2_);

	vec3 GetNormalAt(const vec3 &point);
	float GetIntersection(const Ray &ray, float &u, float &v);

	vec3 v0, v1, v2;
private:
	vec3 normal;
};
