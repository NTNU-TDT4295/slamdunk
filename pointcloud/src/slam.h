#pragma once

#include "window.h"
#include "lidar_socket.h"
#include "hector_slam.h"

struct SlamContext {
	LidarSocketContext lidar_socket;

	unsigned int quad_vao;
	unsigned int texture;

	HectorSlam slam;

	vec2 *points;
	size_t numPoints;
	size_t capPoints;

	struct {
		unsigned int id;
		unsigned int in_matrix;
		unsigned int in_projection_matrix;
		unsigned int in_tex;
	} shader;

	uint8_t *tex_buffer;
};

void init_slam(SlamContext &ctx);
void tick_slam(SlamContext &ctx, const WindowFrameInfo &info);
void free_slam(SlamContext &ctx);
