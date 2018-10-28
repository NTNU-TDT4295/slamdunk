#include "slam.h"
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
// #include "opengl.h"
#include "utils.h"
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

// static int send_point_data(int fd, vec3 point) {
// 	int32_t buffer[3];

// 	if (fd == -1) {
// 		return -1;
// 	}

// 	buffer[0] = (int32_t)(point.x * 1000.0f);
// 	buffer[1] = (int32_t)(point.y * 1000.0f);
// 	buffer[2] = (int32_t)(point.z * 1000.0f);

// 	ssize_t err;
// 	err = send(fd, buffer, sizeof(buffer), 0);
// 	if (err < 0) {
// 		perror("send");
// 		return -1;
// 	}

// 	return 0;
// }

static int send_packet_id(int fd, uint8_t packet_id) {
	if (fd == -1) {
		return -1;
	}

	ssize_t err;
	err = send(fd, &packet_id, sizeof(packet_id), 0);

	if (err < 0) {
		return -1;
	}

	return 0;
}

static int send_map_tile_begin(int fd) {
	return send_packet_id(fd, SLAM_PACKET_TILE_BEGIN);
}

static int send_map_tile_done(int fd) {
	return send_packet_id(fd, SLAM_PACKET_TILE_DONE);
}

static int send_map_tile(int fd, float *map, size_t chunk_x, size_t chunk_y) {
	if (fd == -1) {
		return -1;
	}

	uint8_t buffer[(SLAM_MAP_TILE_SIZE*SLAM_MAP_TILE_SIZE)/4] = {0};

	size_t map_offset =
		(chunk_x * SLAM_MAP_TILE_SIZE) +
		(chunk_y * SLAM_MAP_TILE_SIZE) * SLAM_MAP_WIDTH;

	size_t map_row_stride = SLAM_MAP_WIDTH - SLAM_MAP_TILE_SIZE;

	for (size_t i = 0; i < SLAM_MAP_TILE_SIZE*SLAM_MAP_TILE_SIZE; i++) {
		uint8_t val;

		size_t j = map_offset + i + (i / SLAM_MAP_TILE_SIZE) * map_row_stride;

		if (hs_is_occupied(map[j])) {
			val = 0x2;
		} else if (hs_is_free(map[j])) {
			val = 0x1;
		} else {
			val = 0x0;
		}

		buffer[i/4] |= val << ((j % 4)*2);
	}

	uint8_t header[3];

	header[0] = SLAM_PACKET_TILE;
	header[1] = chunk_x;
	header[2] = chunk_y;

	ssize_t err;
	err = send(fd, header, sizeof(header), 0);

	err = send(fd, buffer, sizeof(buffer), 0);
	if (err < 0) {
		perror("send");
		return -1;
	}

	return 0;
}

static int send_full_map(int fd, float *map) {
	if (fd == -1) {
		return -1;
	}

	send_packet_id(fd, SLAM_PACKET_MAP);

	uint8_t buffer[(SLAM_MAP_WIDTH*SLAM_MAP_HEIGHT)/4] = {0};

	for (size_t i = 0; i < SLAM_MAP_WIDTH*SLAM_MAP_HEIGHT; i++) {
		uint8_t val;
		if (hs_is_occupied(map[i])) {
			val = 0x2;
		} else if (hs_is_free(map[i])) {
			val = 0x1;
		} else {
			val = 0x0;
		}
		buffer[i/4] |= val << ((i % 4)*2);
	}

	size_t bytes_sent = 0;
	ssize_t err;
	while (bytes_sent < sizeof(buffer)) {
		err = send(fd, buffer + bytes_sent,
				   sizeof(buffer) - bytes_sent, 0);
		if (err < 0) {
			perror("send");
			return - 1;
		}
		bytes_sent += err;
	}

	return 0;
}

static int send_map(int fd, float *map, unsigned int *map_last_update, unsigned int last_update) {
	if (fd == -1) {
		return -1;
	}

	constexpr size_t map_area =
		SLAM_MAP_WIDTH * SLAM_MAP_HEIGHT;
	constexpr size_t total_tiles =
		SLAM_MAP_TILES_X * SLAM_MAP_TILES_Y;
	constexpr size_t dirty_tiles_chunks =
		(total_tiles / 64) +
		(((total_tiles % 64) == 0) ? 0 : 1);

	uint64_t dirty_tiles[dirty_tiles_chunks] = {0};
	size_t num_dirty_tiles = 0;

	for (size_t i = 0; i < map_area; i++) {
		if (map_last_update[i] >= last_update) {
			size_t tile_x = (i % SLAM_MAP_WIDTH) / SLAM_MAP_TILE_SIZE;
			size_t tile_y = (i / SLAM_MAP_WIDTH) / SLAM_MAP_TILE_SIZE;
			size_t tile_i = tile_x + tile_y * SLAM_MAP_TILES_X;

			uint64_t mask = 1 << (tile_i % 64);

			// Only increment num_dirty_tiles if this tile was not
			// already marked as dirty.
			num_dirty_tiles += !(dirty_tiles[tile_i / 64] & mask);
			dirty_tiles[tile_i / 64] |= mask;
		}
	}

	if (num_dirty_tiles > total_tiles / 2) {
		printf("send map %zu\n", num_dirty_tiles);
		return send_full_map(fd, map);
	} else if (num_dirty_tiles > 0) {
		size_t err;
		printf("send tiles %zu\n", num_dirty_tiles);

		if (send_map_tile_begin(fd) < 0) {
			return -1;
		}

		for (size_t i = 0; i < total_tiles; i++) {
			if ((dirty_tiles[i / 64] & (1 << (i % 64))) > 0) {
				err = send_map_tile(fd, map, i % SLAM_MAP_TILES_X, i / SLAM_MAP_TILES_X);
				if (err < 0) {
					return err;
				}
			}
		}

		if (send_map_tile_done(fd) < 0) {
			return -1;
		}
	}

	return 0;
}

void init_slam(SlamContext &ctx) {
	ctx.numPoints = 0;
	ctx.capPoints = scan_data_cap;
	ctx.points = (vec2 *)calloc(ctx.capPoints, sizeof(vec2));

	hs_init(ctx.slam);

	ctx.client_fd = -1;

	init_lidar_socket(ctx.lidar_socket, "0.0.0.0", "6002");
}

void tick_slam(SlamContext &ctx, const WindowFrameInfo &info) {
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
		// printf("Updating, %zu\n", ctx.numPoints);
		hs_update(ctx.slam,
				  ctx.points,
				  ctx.numPoints);

		// printf("position: %f %f - %f\n",
		// 	   ctx.slam.lastPosition.x,
		// 	   ctx.slam.lastPosition.y,
		// 	   ctx.slam.lastPosition.z);
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
	}

	if (ctx.client_fd >= 0) {
		if (send_map(ctx.client_fd,
					 ctx.slam.maps[0].values,
					 ctx.slam.maps[0].updateIndex,
					 ctx.last_sent_update) == 0) {
			if ((unsigned int)ctx.slam.maps[0].currentUpdateIndex > ctx.last_sent_update) {
				printf("last sendt update %u\n", ctx.slam.maps[0].currentUpdateIndex);
			}
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
