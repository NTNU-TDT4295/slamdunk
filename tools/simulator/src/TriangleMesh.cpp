#define TINYOBJLOADER_IMPLEMENTATION
#include "TriangleMesh.h"
#include "Object.h"
#include "Triangle.h"
#include "Ray.h"
#include "linmath.h"

#include <glm/glm.hpp>

#include "tiny_obj_loader.h"

TriangleMesh::TriangleMesh(const char *file) {
	std::string inputfile = file;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

	if(!err.empty()) {
		std::cerr << err << std::endl;
	}

	if(!ret) {
		exit(1);
	}

	vec3 vert;
	for(auto &shape : shapes) {
		size_t index_offset = 0;
		for(size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) { // faces (triangles)
			size_t fv = shape.mesh.num_face_vertices[f];
			for(size_t v = 0; v < fv; v++) {
				tinyobj::index_t &idx = shape.mesh.indices[index_offset + v]; // v0
				vert = vec3(attrib.vertices[3 * idx.vertex_index + 0],
							attrib.vertices[3 * idx.vertex_index + 1],
							attrib.vertices[3 * idx.vertex_index + 2]);
				bbox.extendBy(vert);
			}
			index_offset += fv;
		}
	}
}

float TriangleMesh::GetIntersection(const Ray &ray) {
	if (bbox.GetIntersection(ray)) {
		float distLowest = 1000000, intersection;
		bool polygon_hit = false;

		for (auto &shape : shapes) {
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) { // faces (triangles)
				int fv = shape.mesh.num_face_vertices[f];
				tinyobj::index_t &idx0 = shape.mesh.indices[index_offset + 0]; // v0
				tinyobj::index_t &idx1 = shape.mesh.indices[index_offset + 1]; // v1
				tinyobj::index_t &idx2 = shape.mesh.indices[index_offset + 2]; // v2

				// Vertices;
				tri.v0 = vec3(attrib.vertices[3 * idx0.vertex_index + 0],
							  attrib.vertices[3 * idx0.vertex_index + 1],
							  attrib.vertices[3 * idx0.vertex_index + 2]);

				tri.v1 = vec3(attrib.vertices[3 * idx1.vertex_index + 0],
							  attrib.vertices[3 * idx1.vertex_index + 1],
							  attrib.vertices[3 * idx1.vertex_index + 2]);

				tri.v2 = vec3(attrib.vertices[3 * idx2.vertex_index + 0],
							  attrib.vertices[3 * idx2.vertex_index + 1],
							  attrib.vertices[3 * idx2.vertex_index + 2]);

				intersection = tri.GetIntersection(ray, uv.x, uv.y);
				if (intersection) {
					if (intersection < distLowest) {
						polygon_hit = true;
						distLowest = intersection;
					}
				}
				index_offset += fv;
			}
		}
		if (polygon_hit) {
			return distLowest;
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}

vec3 TriangleMesh::GetNormalAt(const vec3&) {
	return normal;
}

vec3 TriangleMesh::GetTexCoords(vec3&, const vec3&) {
	return texCoords;
}
