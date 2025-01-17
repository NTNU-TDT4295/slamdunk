#pragma once

#include "hector_slam_config.h"
#include "linmath.h"
#include <stdint.h>
#include <stddef.h>

constexpr uint8_t SLAM_PACKET_MAP        = 0;
constexpr uint8_t SLAM_PACKET_TILE_BEGIN = 1;
constexpr uint8_t SLAM_PACKET_TILE       = 2;
constexpr uint8_t SLAM_PACKET_TILE_DONE  = 3;
constexpr uint8_t SLAM_PACKET_POSE       = 4;
constexpr uint8_t SLAM_PACKET_RESET_PATH = 5;
constexpr uint8_t SLAM_PACKET_SCAN       = 6;

constexpr size_t SLAM_MAP_TILE_SIZE = 32;

constexpr size_t SLAM_MAP_WIDTH  = HECTOR_SLAM_MAP_WIDTH;
constexpr size_t SLAM_MAP_HEIGHT = HECTOR_SLAM_MAP_HEIGHT;
constexpr float  SLAM_MAP_METERS_PER_PIXEL = HECTOR_SLAM_MAP_CELL_LENGTH;

constexpr size_t SLAM_MAP_TILES_X = SLAM_MAP_WIDTH  / SLAM_MAP_TILE_SIZE;
constexpr size_t SLAM_MAP_TILES_Y = SLAM_MAP_HEIGHT / SLAM_MAP_TILE_SIZE;

int slam_vis_send_map(int fd, float *map, unsigned int *map_last_update, unsigned int last_update);
int slam_vis_send_pose(int fd, vec3 pose);
int slam_vis_send_reset_path(int fd);
int slam_vis_send_scan(int fd, vec2 *scan, size_t scan_length);
