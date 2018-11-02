#pragma once

#include "linmath.h"

constexpr size_t HECTOR_SLAM_MAP_RESOLUTIONS = 3;

struct HectorSlamOccGrid {
	float *values;
	unsigned int *updateIndex;
	size_t width;
	size_t height;
	float cellSize;
	float mapScale;

	int currentUpdateIndex;

	mat3 mapToWorld;
	mat3 worldToMap;
};

struct HectorSlam {
	size_t width;
	size_t height;

	vec3 lastPosition;
	vec3 lastUpdatePosition;

	HectorSlamOccGrid maps[HECTOR_SLAM_MAP_RESOLUTIONS];
};

void hs_init(HectorSlam &slam);
void hs_free(HectorSlam &slam);
void hs_clear(HectorSlam &slam);
void hs_update(HectorSlam &slam, vec2 *points, size_t numPoints);

#define hs_is_occupied(i) (i > 0.0f)
#define hs_is_free(i) (i < 0.0f)
