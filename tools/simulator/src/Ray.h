#pragma once
#include "linmath.h"

struct Ray
{
public:
	Ray();
	Ray(const vec3 origin_, const vec3 direction_);

	void SetOrigin(const vec3 &origin_);
	void SetDirection(const vec3 &direction_);
	vec3 GetOrigin() const;
	vec3 GetDirection() const;

	vec3 invDir;
	float tMin, tMax;
	int sign[3];
private:
	vec3 origin, direction;
};
