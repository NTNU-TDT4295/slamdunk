#include "slam_vis_net.h"
#include "hector_slam.h"
#include <stdio.h>
#include <sys/socket.h>

static int send_packet_id(int fd, uint8_t packet_id) {
	if (fd == -1) {
		return -1;
	}

	ssize_t err;
	err = send(fd, &packet_id, sizeof(packet_id), MSG_NOSIGNAL);

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
	err = send(fd, header, sizeof(header), MSG_NOSIGNAL);

	err = send(fd, buffer, sizeof(buffer), MSG_NOSIGNAL);
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
				   sizeof(buffer) - bytes_sent, MSG_NOSIGNAL);
		if (err < 0) {
			perror("send");
			return - 1;
		}
		bytes_sent += err;
	}

	return 0;
}

int slam_vis_send_map(int fd, float *map, unsigned int *map_last_update, unsigned int last_update) {
	if (fd == -1) {
		return -1;
	}

	constexpr size_t word_size = sizeof(uintmax_t) * 8;

	constexpr size_t map_area =
		SLAM_MAP_WIDTH * SLAM_MAP_HEIGHT;
	constexpr size_t total_tiles =
		SLAM_MAP_TILES_X * SLAM_MAP_TILES_Y;
	constexpr size_t dirty_tiles_chunks =
		(total_tiles / word_size) +
		(((total_tiles % word_size) == 0) ? 0 : 1);

	uintmax_t dirty_tiles[dirty_tiles_chunks] = {0};
	size_t num_dirty_tiles = 0;

	for (size_t i = 0; i < map_area; i++) {
		if (map_last_update[i] >= last_update) {
			size_t tile_x = (i % SLAM_MAP_WIDTH) / SLAM_MAP_TILE_SIZE;
			size_t tile_y = (i / SLAM_MAP_WIDTH) / SLAM_MAP_TILE_SIZE;
			size_t tile_i = tile_x + tile_y * SLAM_MAP_TILES_X;

			uintmax_t mask = (uintmax_t)1 << (tile_i % word_size);

			// Only increment num_dirty_tiles if this tile was not
			// already marked as dirty.
			num_dirty_tiles += !(dirty_tiles[tile_i / word_size] & mask);
			dirty_tiles[tile_i / word_size] |= mask;
		}
	}

	printf("%zu\n", num_dirty_tiles);
	if (num_dirty_tiles > total_tiles / 2) {
		return send_full_map(fd, map);
	} else if (num_dirty_tiles > 0) {
		size_t err;
		if (send_map_tile_begin(fd) < 0) {
			return -1;
		}

		size_t tiles_updated = 0;

		for (size_t i = 0; i < total_tiles; i++) {
			if ((dirty_tiles[i / word_size] & ((uintmax_t)1 << (i % word_size))) > 0) {
				err = send_map_tile(fd, map, i % SLAM_MAP_TILES_X, i / SLAM_MAP_TILES_X);
				tiles_updated += 1;
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

int slam_vis_send_pose(int fd, vec3 pose) {
	if (fd == -1) {
		return -1;
	}

	uint8_t buffer[1 + 3*sizeof(int32_t)];
	int32_t *pos_buffer = (int32_t *)&buffer[1];

	buffer[0] = SLAM_PACKET_POSE;
	pos_buffer[0] = pose.x * 1000.0f;
	pos_buffer[1] = pose.y * 1000.0f;
	pos_buffer[2] = pose.z * 1000000.0f;

	ssize_t err;
	err = send(fd, buffer, sizeof(buffer), MSG_NOSIGNAL);
	if (err < (ssize_t)sizeof(buffer)) {
		return -1;
	}

	return 0;
}

int slam_vis_send_reset_path(int fd) {
	return send_packet_id(fd, SLAM_PACKET_RESET_PATH);
}
