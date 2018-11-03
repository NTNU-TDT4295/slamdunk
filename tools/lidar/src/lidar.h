#pragma once

#include "window.h"
#include "lidar_socket.h"
#include <stddef.h>

struct LidarContext {
	unsigned int buffer_vbo;
	unsigned int buffer_vao;

	struct {
		unsigned int id;
		unsigned int scale;
		unsigned int color;
	} shader;

	LidarSocketContext lidar_socket;
};

void init_lidar(LidarContext &ctx);
void tick_lidar(LidarContext &ctx, const WindowFrameInfo &frame);
void free_lidar(LidarContext &ctx);
