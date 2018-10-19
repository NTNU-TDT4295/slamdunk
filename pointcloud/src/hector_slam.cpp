#define GLM_ENABLE_EXPERIMENTAL

#include "hector_slam.h"
#include <glm/gtx/matrix_transform_2d.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

static inline int sign(int x)
{
	return x > 0 ? 1 : -1;
}

static inline float normalize_angle(float angle)
{
	float a = fmod(fmod(angle, 2.0f*M_PI) + 2.0f*M_PI, 2.0f*M_PI);
	if (a > M_PI){
		a -= 2.0f*M_PI;
	}
	return a;
}

static float hs_log_odds(float val) {
	float odds = exp(val);
	return odds / (odds + 1.0f);
}

constexpr float hs_rad_to_deg(float rad) {
	return rad * 180.0f / M_PI;
}

constexpr float hs_deg_to_rad(float deg) {
	return deg * M_PI / 180.0f;
}

constexpr float hs_prob_to_log_odds(float prob) {
	return log(prob / (1.0f - prob));
}

constexpr float log_odds_free = hs_prob_to_log_odds(HECTOR_SLAM_UPDATE_FREE_FACTOR);
constexpr float log_odds_occupied = hs_prob_to_log_odds(HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR);

static inline vec3 pose_position_affine(vec3 in) {
	return vec3(in.x, in.y, 1.0f);
}

static vec3 hs_sample_map_value_with_derivatives(const HectorSlamOccGrid &map,
													 const vec2& coords) {
	// if out of bonuds
    vec2i indMin = vec2i((int)coords.x, (int)coords.y);

	if (coords.x < 0 || coords.x >= map.width ||
		coords.y < 0 || coords.y >= map.height) {
		return vec3(0.0f, 0.0f, 0.0f);
	}

	vec2 factors = coords - vec2((float)indMin.x, (float)indMin.y);

	int sizeX = map.width;
	int index = indMin.x * sizeX + indMin.y;

	float intensities[4];

	intensities[0] = hs_log_odds(map.values[index]);
	intensities[1] = hs_log_odds(map.values[index+1]);
	intensities[2] = hs_log_odds(map.values[index+map.width]);
	intensities[3] = hs_log_odds(map.values[index+map.width+1]);

    float dx1 = intensities[0] - intensities[1];
    float dx2 = intensities[2] - intensities[3];

    float dy1 = intensities[0] - intensities[2];
    float dy2 = intensities[1] - intensities[3];

    float xFacInv = (1.0f - factors[0]);
    float yFacInv = (1.0f - factors[1]);

    return vec3(
      ((intensities[0] * xFacInv + intensities[1] * factors[0]) * (yFacInv)) +
      ((intensities[2] * xFacInv + intensities[3] * factors[0]) * (factors[1])),
      -((dx1 * xFacInv) + (dx2 * factors[0])),
      -((dy1 * yFacInv) + (dy2 * factors[1]))
    );
}

static mat3 hs_transform_from_pose(const vec3 &pose) {
	mat3 matrix;
	matrix = glm::translate(mat3(1.0f), vec2(pose.x, pose.y));
	matrix = glm::rotate(matrix, hs_rad_to_deg(pose[2]));
	return matrix;
}

struct HessianDerivs {
	mat3 H;
	vec3 dTr;
};

static HessianDerivs hs_get_complete_hessian_derivs(const HectorSlamOccGrid &map, const vec3 &pose, vec2 *points, size_t numPoints) {
	HessianDerivs result;
	result.H = mat3(0.0f);
	result.dTr = vec3(0.0f);

    mat3 transform = hs_transform_from_pose(pose);

    float sinRot = sin(pose[2]);
    float cosRot = cos(pose[2]);

	for (size_t i = 0; i < numPoints; i++) {
		vec2 point = points[i] * map.scaleFactor;

		vec3 pointData =
			hs_sample_map_value_with_derivatives(map, transform * vec3(point, 1.0f));

		float funVal = 1.0f - pointData[0];

		result.dTr[0] += pointData[1] * funVal;
		result.dTr[1] += pointData[2] * funVal;

		float rotDeriv =
			((-sinRot * point.x - cosRot * point.y) * pointData[1] +
			 ( cosRot * point.x - sinRot * point.y) * pointData[2]);

		result.dTr[2] += rotDeriv * funVal;

		result.H[0][0] += pointData[1] * pointData[1];
		result.H[1][1] += pointData[2] * pointData[2];
		result.H[2][2] += rotDeriv * rotDeriv;

		result.H[0][1] += pointData[1] * pointData[2];
		result.H[0][2] += pointData[1] * rotDeriv;
		result.H[1][2] += pointData[2] * rotDeriv;
	}

    result.H[1][0] = result.H[0][1];
    result.H[2][0] = result.H[0][2];
    result.H[2][1] = result.H[1][2];

	return result;
}

static bool hs_estimate_transformation_log_lh(vec3 &estimate,
									   const HectorSlamOccGrid &map,
									   vec2 *points, size_t numPoints) {
	HessianDerivs hessian =
		hs_get_complete_hessian_derivs(map, estimate, points, numPoints);

	if ((hessian.H[0][0] != 0.0f) && (hessian.H[1][1] != 0.0f)) {
		vec3 searchDir = glm::inverse(hessian.H) * hessian.dTr;

		if (searchDir[2] > 0.2f) {
			searchDir[2] = 0.2f;
			printf("SearchDir angle change too large\n");
		} else if (searchDir[2] < -0.2f) {
			searchDir[2] = -0.2f;
			printf("SearchDir angle change too large\n");
		}

		estimate += searchDir;
		return true;
	}
	return false;
}

static vec3 hs_get_map_coords_pose(const HectorSlamOccGrid &map, const vec3 &worldPose) {
    vec2 mapCoords = map.mapTworld * pose_position_affine(worldPose);
    return vec3(mapCoords[0], mapCoords[1], worldPose[2]);
}

static vec3 hs_get_world_coords_pose(const HectorSlamOccGrid &map, const vec3 &mapPose) {
    vec2 worldCoords = glm::inverse(map.mapTworld) * pose_position_affine(mapPose);
    return vec3(worldCoords[0], worldCoords[1], mapPose[2]);
}

static vec3 hs_match_data(const HectorSlamOccGrid &map,
				   const vec3 &beginEstimateWorld,
				   int numIter,
				   vec2 *points, size_t numPoints) {
	vec3 estimate = hs_get_map_coords_pose(map, beginEstimateWorld);

	for (int i = 0; i < numIter; i++) {
		hs_estimate_transformation_log_lh(estimate, map, points, numPoints);
	}

	estimate[2] = normalize_angle(estimate[2]);

	return hs_get_world_coords_pose(map, estimate);
}

void hs_init(HectorSlam &slam) {
	slam.width = 1024;
	slam.height = 1024;

	slam.updateIndex = 1;

	slam.lastUpdatePosition = vec3(FLT_MAX);
	slam.lastPosition = vec3(0.0f);

	slam.maps[0].width = slam.width;
	slam.maps[0].height = slam.height;
	slam.maps[0].scaleFactor = 1.0f;
	slam.maps[0].cellSize = 0.025f;

	for (size_t i = 1; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		slam.maps[i].width  = slam.maps[i-1].width  / 2;
		slam.maps[i].height = slam.maps[i-1].height / 2;
		slam.maps[i].scaleFactor = slam.maps[i-1].scaleFactor / 2.0f;
		slam.maps[i].cellSize = slam.maps[i-1].cellSize * 2.0f;
	}

	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		size_t numPixels = slam.maps[i].width * slam.maps[i].height;
		slam.maps[i].values =
			(float *)calloc(numPixels, sizeof(float));
		slam.maps[i].updateIndex =
			(unsigned int *)calloc(numPixels, sizeof(unsigned int));

		float scaleToMap = 1.0f / slam.maps[i].cellSize;

		slam.maps[i].mapTworld =
			glm::translate(mat3(1.0f),
						   vec2((float)slam.maps[i].width  / 2.0f,
								(float)slam.maps[i].height / 2.0f));
		slam.maps[i].mapTworld =
			glm::scale(slam.maps[i].mapTworld, vec2(scaleToMap));
	}
}

static bool hs_pose_difference_larger_than(const vec3& pose1,
										   const vec3& pose2,
										   float distanceDiffThresh,
										   float angleDiffThresh) {
	if ((glm::length(vec2(pose1) - vec2(pose2))) > distanceDiffThresh) {
		return true;
	}

	float angleDiff = (pose1.z - pose2.z);

	if (angleDiff > M_PI) {
		angleDiff -= M_PI * 2.0f;
	} else if (angleDiff < -M_PI) {
		angleDiff += M_PI * 2.0f;
	}

	if (fabs(angleDiff) > angleDiffThresh){
		return true;
	}

	return false;
}

inline static void hs_mark_cell_free(HectorSlam &slam, HectorSlamOccGrid &map, unsigned int i) {
	if (map.updateIndex[i] < slam.updateIndex+0) {
		map.values[i] += log_odds_free;
		map.updateIndex[i] = slam.updateIndex;
	}
}

inline static void hs_mark_cell_occ(HectorSlam &slam, HectorSlamOccGrid &map, unsigned int i) {
	if (map.updateIndex[i] < slam.updateIndex+1) {
		if (map.updateIndex[i] == slam.updateIndex+0) {
			map.values[i] -= log_odds_free;
		}

		if (map.values[i] < 50.0f) {
			map.values[i] += log_odds_occupied;
		}
		map.updateIndex[i] = slam.updateIndex + 1;
	}
}

inline void hs_bresenham2D(HectorSlam &slam,
						   HectorSlamOccGrid &map,
						   unsigned int abs_da,
						   unsigned int abs_db,
						   int error_b,
						   int offset_a,
						   int offset_b,
						   unsigned int offset) {

    hs_mark_cell_free(slam, map, offset);

	unsigned int end = abs_da-1;

	for(unsigned int i = 0; i < end; ++i){
		offset += offset_a;
		error_b += abs_db;

		if((unsigned int)error_b >= abs_da){
			offset += offset_b;
			error_b -= abs_da;
		}

		hs_mark_cell_free(slam, map, offset);
	}
}

inline static void hs_update_line_bresenhami(HectorSlam &slam,
											 HectorSlamOccGrid &map,
											 const vec2i& begin,
											 const vec2i& end) {
	int x0 = begin[0];
	int y0 = begin[1];

	if ((x0 < 0) || (x0 >= (int)map.width) || (y0 < 0) || (y0 >= (int)map.height)) {
		return;
	}

    int x1 = end[0];
    int y1 = end[1];

	if ((x1 < 0) || (x1 >= (int)map.width) || (y1 < 0) || (y1 >= (int)map.height)) {
		return;
	}

    int dx = x1 - x0;
    int dy = y1 - y0;

    unsigned int abs_dx = abs(dx);
    unsigned int abs_dy = abs(dy);

    int offset_dx = sign(dx);
    int offset_dy = sign(dy) * map.width;

    unsigned int startOffset = begin.y * map.width + begin.x;

    //if x is dominant
    if(abs_dx >= abs_dy){
      int error_y = abs_dx / 2;
      hs_bresenham2D(slam, map, abs_dx, abs_dy, error_y, offset_dx, offset_dy, startOffset);
    }else{
      //otherwise y is dominant
      int error_x = abs_dy / 2;
      hs_bresenham2D(slam, map, abs_dy, abs_dx, error_x, offset_dy, offset_dx, startOffset);
    }

    unsigned int endOffset = end.y * map.width + end.x;
    hs_mark_cell_occ(slam, map, endOffset);
}

void hs_update_map(HectorSlam &slam,
				   const vec3 &pose,
				   vec2 *points, size_t numPoints) {
	for (size_t map_i = 0; map_i < HECTOR_SLAM_MAP_RESOLUTIONS; map_i++) {
		HectorSlamOccGrid *map = &slam.maps[map_i];
		vec3 mapPose = hs_get_map_coords_pose(*map, pose);
		vec2 position = vec2(mapPose);
		vec2i positioni = vec2i(position.x + 0.5f,
								position.y + 0.5f);

		mat3 poseTransform;
		poseTransform = glm::translate(mat3(1.0f),
									   vec2(mapPose.x, mapPose.y));
		poseTransform = glm::rotate(poseTransform, hs_rad_to_deg(mapPose[2]));
		poseTransform = glm::scale(poseTransform, vec2(map->scaleFactor));

		for (size_t i = 0; i < numPoints; i++) {
			vec2 point = poseTransform * vec3(points[i], 1.0f);
			point.x += 0.5f;
			point.y += 0.5f;

			vec2i pointi = point;

			if (pointi != positioni) {
				hs_update_line_bresenhami(slam, *map, positioni, pointi);
			}
		}
	}

	slam.updateIndex += 2;
}

void hs_update(HectorSlam &slam, vec2 *points, size_t numPoints) {
	vec3 estimate = slam.lastPosition;

	for (int i = HECTOR_SLAM_MAP_RESOLUTIONS - 1; i >= 0; i--) {
		estimate =
			hs_match_data(slam.maps[i], estimate,
						    (i == 0)
						  ? HECTOR_SLAM_ITERATIONS_FINAL
						  : HECTOR_SLAM_ITERATIONS,
						  points, numPoints);
	}

	slam.lastPosition = estimate;

	if (hs_pose_difference_larger_than(estimate, slam.lastUpdatePosition,
									   HECTOR_SLAM_DISTANCE_THRESHOLD,
									   HECTOR_SLAM_ANGLE_THRESHOLD)) {
		hs_update_map(slam, estimate, points, numPoints);
		slam.lastUpdatePosition = estimate;
	}
}

void hs_free(HectorSlam &slam) {
	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		free(slam.maps[i].values);
		free(slam.maps[i].updateIndex);
	}
}
