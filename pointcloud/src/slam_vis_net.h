#pragma once

constexpr uint8_t SLAM_PACKET_MAP        = 0;
constexpr uint8_t SLAM_PACKET_TILE_BEGIN = 1;
constexpr uint8_t SLAM_PACKET_TILE       = 2;
constexpr uint8_t SLAM_PACKET_TILE_DONE  = 3;
constexpr uint8_t SLAM_PACKET_POSE       = 4;

constexpr size_t SLAM_MAP_TILE_SIZE = 32;

constexpr size_t SLAM_MAP_WIDTH  = 1024;
constexpr size_t SLAM_MAP_HEIGHT = 1024;

constexpr size_t SLAM_MAP_TILES_X = SLAM_MAP_WIDTH  / SLAM_MAP_TILE_SIZE;
constexpr size_t SLAM_MAP_TILES_Y = SLAM_MAP_HEIGHT / SLAM_MAP_TILE_SIZE;
