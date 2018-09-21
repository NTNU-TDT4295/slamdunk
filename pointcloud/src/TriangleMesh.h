#pragma once
#define TINYOBJLOADER_IMPLEMENTATION
#include "Triangle.h"
#include "linmath.h"
#include "tiny_obj_loader.h"
#include "Globals.h"
#include <iostream>
#include <memory>

class TriangleMesh: public Object
{
public:
	TriangleMesh(const char *file);
	float GetIntersection(const Ray &ray);
	vec3 GetNormalAt(const vec3 &intersectionPosition);
	vec3 GetTexCoords(vec3 &normal, const vec3 &hitPoint);

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	Triangle tri;
	vec3 n0, n1, n2, normal;
	vec3 uv;
	vec3 st0, st1, st2, texCoords;
};
