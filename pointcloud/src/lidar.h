#pragma once

#include "window.h"
#include "net.h"
#include "linmath.h"
#include <stddef.h>
#include <semaphore.h>

constexpr size_t scan_data_cap = 512;

struct LidarContext {
	vec2 scan_data[2][scan_data_cap];
	size_t scan_data_length[2];

	int scan_data_read_select;
	sem_t lock;

	unsigned int buffer_vbo;
	unsigned int buffer_vao;

	net_context net_in;

	struct {
		unsigned int id;
		unsigned int scale;
		unsigned int color;
	} shader;
};

void init_lidar(LidarContext &ctx);
void tick_lidar(LidarContext &ctx, const WindowFrameInfo &frame);
void free_lidar(LidarContext &ctx);
