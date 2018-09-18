#pragma once
#include <stdlib.h>

struct OctreePoint {
	float x, y, z;
};

struct OctreeBoundingBox {
	OctreePoint center;
	float radius;
};

struct OctreeNode;

constexpr size_t OCTREE_CHILDREN = 8;
constexpr size_t OCTREE_LEAFS = (sizeof(OctreeNode *) * OCTREE_CHILDREN) / sizeof(OctreePoint);

struct OctreeNode {
	OctreeBoundingBox box;
	size_t num_points;
	bool dirty;
	unsigned int buffer_id;
	unsigned int vao_id;

	union {
		OctreePoint leafs[OCTREE_LEAFS];
		OctreeNode *children[OCTREE_CHILDREN];
	};
};

struct Octree {
	OctreeBoundingBox box;
	struct OctreeNode *root;

	void insert(OctreePoint);
};
