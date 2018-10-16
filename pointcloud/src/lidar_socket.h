#pragma once

#include "net.h"
#include "linmath.h"
#include <semaphore.h>

constexpr size_t scan_data_cap = 512;

struct LidarSocketContext {
	vec2 scan_data[2][scan_data_cap];
	size_t scan_data_length[2];

	int scan_data_read_select;
	sem_t lock;

	net_context net_in;
};

void init_lidar_socket(LidarSocketContext &ctx, const char *node, const char *service);
void free_lidar_socket(LidarSocketContext &ctx);
