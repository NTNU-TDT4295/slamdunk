#include "octree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <cmath>

constexpr float merge_limit = 0.0001f;

static inline uint8_t octree_point_next_parent(OctreeBoundingBox box, OctreePoint point) {
	uint8_t result = 0;

	if (point.x >= box.center.x) result |= 1;
	if (point.y >= box.center.y) result |= 2;
	if (point.z >= box.center.z) result |= 4;

	return result;
}

static inline OctreeBoundingBox octree_child_box(OctreeBoundingBox box, uint8_t id) {
	constexpr OctreePoint offset[] = {
		{ -1.0f, -1.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f },
		{ -1.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f, -1.0f },

		{ -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
	};

	OctreeBoundingBox result;

	result.radius = box.radius / 2.0f;
	result.center.x = box.center.x + offset[id].x * result.radius;
	result.center.y = box.center.y + offset[id].y * result.radius;
	result.center.z = box.center.z + offset[id].z * result.radius;

	return result;
}

static void octree_node_insert(OctreeNode **node, OctreePoint point, OctreeBoundingBox box);

static void split_node(OctreeNode *node) {
	OctreeNode *children[OCTREE_CHILDREN] = {0};

	// Subtract 1 to compasate for the new element that is to be
	// inserted, but is not yet actually inserted. @TODO: Make this
	// better.
	for (size_t i = 0; i < node->num_points - 1; i++) {
		uint8_t child_id = octree_point_next_parent(node->box, node->leafs[i]);
		OctreeBoundingBox child_box = octree_child_box(node->box, child_id);

		octree_node_insert(&children[child_id], node->leafs[i], child_box);
	}

	memcpy(node->children, children, sizeof(node->children));
}

static void octree_node_insert(OctreeNode **node, OctreePoint point, OctreeBoundingBox box) {
	if (!*node) {
		*node = (OctreeNode *)calloc(1, sizeof(OctreeNode));
		(*node)->box = box;
	}

	(*node)->num_points += 1;
	(*node)->dirty = true;

	while ((*node)->num_points > OCTREE_LEAFS) {
		if ((*node)->num_points == OCTREE_LEAFS + 1) {
			split_node(*node);
		}

		uint8_t next_box = octree_point_next_parent((*node)->box, point);
		OctreeBoundingBox previous_box = (*node)->box;
		node = &(*node)->children[next_box];

		if (!*node) {
			*node = (OctreeNode *)calloc(1, sizeof(OctreeNode));
			(*node)->box = octree_child_box(previous_box, next_box);
		}

		(*node)->num_points += 1;
		(*node)->dirty = true;
	}

	if (point.x < (*node)->box.center.x - (*node)->box.radius ||
		point.x >=  (*node)->box.center.x + (*node)->box.radius ||
		point.y < (*node)->box.center.y - (*node)->box.radius ||
		point.y >=  (*node)->box.center.y + (*node)->box.radius ||
		point.z < (*node)->box.center.z - (*node)->box.radius ||
		point.z >=  (*node)->box.center.z + (*node)->box.radius) {
		// printf("Point discarded (%f %f %f)\n", point.x, point.y, point.z);
		return;
	}

	// assert(point.x >= (*node)->box.center.x - (*node)->box.radius);
	// assert(point.x <  (*node)->box.center.x + (*node)->box.radius);

	// assert(point.y >= (*node)->box.center.y - (*node)->box.radius);
	// assert(point.y <  (*node)->box.center.y + (*node)->box.radius);

	// assert(point.z >= (*node)->box.center.z - (*node)->box.radius);
	// assert(point.z <  (*node)->box.center.z + (*node)->box.radius);

	(*node)->leafs[(*node)->num_points - 1] = point;
}

static bool octree_has_equal_point(OctreeNode *node, OctreePoint point) {
	if (!node) {
		return false;
	}
	if (node->num_points > OCTREE_LEAFS) {
		uint8_t next_box = octree_point_next_parent(node->box, point);

		return octree_has_equal_point(node->children[next_box], point);
	} else {
		for (size_t i = 0; i < node->num_points; i++) {
			if (fabsf(node->leafs[i].x - point.x) < merge_limit &&
				fabsf(node->leafs[i].y - point.y) < merge_limit &&
				fabsf(node->leafs[i].z - point.z) < merge_limit) {
				return true;
			}
		}
	}

	return false;
}

void Octree::insert(OctreePoint point) {
	OctreeNode **node;
	node = &this->root;

	// Filter out points that are equal to existing points.
	if (octree_has_equal_point(*node, point)) {
		return;
	}

	octree_node_insert(node, point, this->box);
}
