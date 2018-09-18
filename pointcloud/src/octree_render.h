#pragma once
#include "octree.h"

struct OctreeRender {
	Octree octree;
	unsigned int cube_vao;
	unsigned int matrix_uniform;
};

void octree_render_init(OctreeRender &);
void octree_render(OctreeRender &);
void octree_render_bounds(OctreeRender &);
