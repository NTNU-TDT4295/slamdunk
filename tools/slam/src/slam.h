#pragma once

#include "lidar_socket.h"
#include "hector_slam.h"
#include <time.h>

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

	bool btn_down;
	int res;

	uint8_t *tex_buffer;

	unsigned int last_sent_update;

	int client_fd;
	struct timespec last_reconnect;
};

void init_slam(SlamContext &ctx);
void tick_slam(SlamContext &ctx);
void free_slam(SlamContext &ctx);
