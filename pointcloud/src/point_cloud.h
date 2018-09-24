#pragma once

#include "octree.h"
#include "octree_render.h"
#include "window.h"
#include "linmath.h"
#include "camera.h"

#include "net.h"
#include "pointqueue.h"

struct PointCloudContext {
	Camera camera;

	struct {
		unsigned int id;
		unsigned int in_matrix;
		unsigned int in_projection_matrix;
		unsigned int emission_color;
		unsigned int diffuse_color;
	} shader;

	OctreeRender octree_render;
	bool display_octree_boundaries;
	bool display_octree_boundaries_down;

	unsigned int axis_vao;
	unsigned int axis_vbo;

	net_context net;

	PointQueue queue;
};

void init_point_cloud(PointCloudContext &);
void free_point_cloud(PointCloudContext &);
void tick_point_cloud(PointCloudContext &, const WindowFrameInfo &);
