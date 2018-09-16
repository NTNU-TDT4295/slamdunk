#pragma once

#include "octree.h"
#include "window.h"
#include "linmath.h"
#include "model_loader_obj.h"

struct PointCloudContext {
	struct {
		vec3 position;
		float yaw, pitch;
	} camera;

	struct {
		unsigned int id;
		unsigned int in_matrix;
		unsigned int in_projection_matrix;
		unsigned int emission_color;
		unsigned int diffuse_color;
	} shader;

	model mdl;
};

void init_point_cloud(PointCloudContext &);
void tick_point_cloud(PointCloudContext &, const WindowFrameInfo &);
