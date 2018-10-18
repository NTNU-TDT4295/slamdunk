#pragma once

#include "window.h"
#include "lidar_socket.h"

struct InternalSlamContext;

struct SlamContext {
	InternalSlamContext *internal;
	LidarSocketContext lidar_socket;

	unsigned int quad_vao;
	unsigned int texture;

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
