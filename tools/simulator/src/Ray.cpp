#include "Ray.h"

constexpr float BIAS = 1e-8;

Ray::Ray()
{
	origin = vec3(0);
	direction = vec3(0, 0, 1);
	tMin = BIAS;
	tMax = 1000;

	invDir.x = 1 / direction.x;
	invDir.y = 1 / direction.y;
	invDir.z = 1 / direction.z;

	sign[0] = (invDir.x < 0);
	sign[1] = (invDir.y < 0);
	sign[2] = (invDir.z < 0);
}

Ray::Ray(const vec3 origin_, const vec3 direction_)
{
	origin = origin_;
	direction = direction_;
	tMin = BIAS;
	tMax = 100000;

	invDir.x = 1 / direction_.x;
	invDir.y = 1 / direction_.y;
	invDir.z = 1 / direction_.z;

	sign[0] = (invDir.x < 0);
	sign[1] = (invDir.y < 0);
	sign[2] = (invDir.z < 0);
}

void Ray::SetOrigin(const vec3 &origin_)
{
	origin = origin_;
}

void Ray::SetDirection(const vec3 &direction_)
{
	direction = direction_;
}

vec3 Ray::GetOrigin() const
{
	return origin;
}

vec3 Ray::GetDirection() const
{
	return direction;
}
