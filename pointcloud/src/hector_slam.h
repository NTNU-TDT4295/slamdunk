#pragma once

#include "linmath.h"

constexpr size_t HECTOR_SLAM_MAP_RESOLUTIONS = 3;

constexpr float HECTOR_SLAM_UPDATE_FREE_FACTOR = 0.4f;
constexpr float HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR = 0.9f;

constexpr float HECTOR_SLAM_DISTANCE_THRESHOLD = 0.4f;
constexpr float HECTOR_SLAM_ANGLE_THRESHOLD = 0.9f;

constexpr int HECTOR_SLAM_ITERATIONS = 4;
constexpr int HECTOR_SLAM_ITERATIONS_FINAL = 6;


struct HectorSlamOccGrid {
	float *values;
	size_t width;
	size_t height;
	float cellSize;

	void *container;

	void *gridMap;
	void *util;
};

struct HectorSlam {
	void *cont;
	void *scanMatcher;

	size_t width;
	size_t height;

	vec3 lastPosition;
	vec3 lastUpdatePosition;

	HectorSlamOccGrid maps[HECTOR_SLAM_MAP_RESOLUTIONS];
};

void hs_init(HectorSlam &slam);
void hs_free(HectorSlam &slam);
void hs_update(HectorSlam &slam, vec2 *points, size_t numPoints);

#define hs_is_occupied(i) (i > 0.0f)
#define hs_is_free(i) (i < 0.0f)
