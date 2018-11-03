#pragma once
#include "linmath.h"
#include "Ray.h"
#include "BBox.h"

class Object
{
public:
	Object() noexcept;
	virtual ~Object();

	virtual float GetIntersection(const Ray &ray);
	virtual vec3 GetNormalAt(const vec3 &intersectionPosition);
	virtual vec3 GetTexCoords(vec3 &normal, const vec3 &hitPoint);

	BBox bbox;
};
