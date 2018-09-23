#include "Triangle.h"
#include "linmath.h"
#include <glm/glm.hpp>

Triangle::Triangle()
{
	v0 = vec3(0, 0, 6);
	v1 = vec3(1, 0, 6);
	v2 = vec3(1, 1, 6);
	normal = glm::normalize(glm::cross((v1 - v0),(v2 - v0)));
}

Triangle::Triangle(vec3 &v0_, vec3 &v1_, vec3 &v2_)
  :v0 {v0_}, v1 {v1_}, v2 {v2_}, normal {glm::normalize(glm::cross((v1 - v0),(v2 - v0)))}
{}

vec3 Triangle::GetNormalAt(const vec3 &)
{
	return normal; // Has to be inverted for some reason
}

constexpr float BIAS = 1e-8;

float Triangle::GetIntersection(const Ray &ray, float &u, float &v)
{
	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = glm::cross(ray.GetDirection(), v0v2);
	float det = glm::dot(v0v1, pvec);

	// ray and triangle are parallel if det is close to 0
	if(fabsf(det) < BIAS) return false;

	float invDet = 1 / det;

	vec3 tvec = ray.GetOrigin() - v0;
	u = glm::dot(tvec, pvec) * invDet;
	if(u < 0 || u > 1) return false;

	vec3 qvec = glm::cross(tvec, v0v1);
	v = glm::dot(ray.GetDirection(), qvec) * invDet;
	if(v < 0 || u + v > 1) return false;

	float t = glm::dot(v0v2, qvec) * invDet;

	return (t > BIAS) ? t : false;
}

