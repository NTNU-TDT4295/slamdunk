#include "slam.h"
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include "slam_vis_net.h"

#include <glm/gtc/type_ptr.hpp>

static double timespec_diff_to_sec(const timespec &from, const timespec &to) {
    timespec result;    if ((to.tv_nsec - from.tv_nsec) < 0) {
       result.tv_sec = to.tv_sec - from.tv_sec - 1;
       result.tv_nsec = to.tv_nsec - from.tv_nsec + 1000000000;
   } else {
       result.tv_sec = to.tv_sec - from.tv_sec;
       result.tv_nsec = to.tv_nsec - from.tv_nsec;
   }    return result.tv_sec + (double)result.tv_nsec / 1000000000.0;
}

static struct timespec read_time() {
	struct timespec time;
	int error;

	error = clock_gettime(CLOCK_MONOTONIC, &time);
	assert(!error);

	return time;
}

void init_slam(SlamContext &ctx) {
	ctx.numPoints = 0;
	ctx.capPoints = scan_data_cap;
	ctx.points = (vec2 *)calloc(ctx.capPoints, sizeof(vec2));

	hs_init(ctx.slam);

	ctx.client_fd = -1;

	init_lidar_socket(ctx.lidar_socket, "0.0.0.0", "6002");
}

void tick_slam(SlamContext &ctx) {
	sem_wait(&ctx.lidar_socket.lock);

	size_t length = ctx.lidar_socket.scan_data_length[ctx.lidar_socket.scan_data_read_select];

	float scale = 1.0f / ctx.slam.maps[0].cellSize;

	ctx.numPoints = 0;

	for (size_t i = 0; i < length; i++) {
		vec2 p = ctx.lidar_socket.scan_data[ctx.lidar_socket.scan_data_read_select][i];
		float angle = p.x * M_PI / 180.0f;
		float dist  = (p.y / 1000.0f) * scale;

		if (dist > 0.1f) {
			if (ctx.numPoints >= ctx.capPoints) {
				printf("Too many points in tick!\n");
				break;
			}

			vec2 point = vec2(cos(angle) * dist, sin(angle) * dist);
			ctx.points[ctx.numPoints] = point;
			ctx.numPoints += 1;
		}
	}

	sem_post(&ctx.lidar_socket.lock);

	if (ctx.numPoints > 0) {
		hs_update(ctx.slam,
				  ctx.points,
				  ctx.numPoints);

		slam_vis_send_pose(ctx.client_fd, ctx.slam.lastPosition);
	}

	struct timespec time_current;
	time_current = read_time();

	if (ctx.client_fd < 0 && timespec_diff_to_sec(ctx.last_reconnect, time_current) > 1.0) {
		printf("Attempting to connect... ");
		fflush(stdout);
		ctx.client_fd = net_client_connect("127.0.0.1", "6000");
		if (ctx.client_fd >= 0) {
			printf("done!\n");
		}
		ctx.last_reconnect = time_current;
		ctx.last_sent_update = 0;
	}

	if (ctx.client_fd >= 0 && ctx.last_sent_update < (unsigned int)ctx.slam.maps[0].currentUpdateIndex) {
		if (slam_vis_send_map(ctx.client_fd,
							  ctx.slam.maps[0].values,
							  ctx.slam.maps[0].updateIndex,
							  ctx.last_sent_update) == 0) {
			ctx.last_sent_update = ctx.slam.maps[0].currentUpdateIndex;
		} else {
			ctx.client_fd = -1;
			ctx.last_reconnect = time_current;
		}
	}
}

void free_slam(SlamContext &ctx) {
	free_lidar_socket(ctx.lidar_socket);
	hs_free(ctx.slam);

	free(ctx.points);
	free(ctx.tex_buffer);
}
