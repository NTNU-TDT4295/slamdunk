#define GLM_ENABLE_EXPERIMENTAL

#include "hector_slam.h"
#include <glm/gtx/matrix_transform_2d.hpp>
#include <iostream>

#include <float.h>
#include <limits.h>
#include <string.h>

#ifdef PROFILING
    #include <time.h>
    #include "profiling.h"
    #define BEGIN_PROFILING_BLOCK(name)

    // timing hs_match_data();
    double acc_mdata_time[HECTOR_SLAM_MAP_RESOLUTIONS] = {0.0};
    int mdata_iterations[HECTOR_SLAM_MAP_RESOLUTIONS] = {0};

    // timing hs_update_map_by_scan();
    double acc_umapbs_time[HECTOR_SLAM_MAP_RESOLUTIONS] = {0.0};
    int umapbs_iterations[HECTOR_SLAM_MAP_RESOLUTIONS] = {0};

    //avg time of hs_update():
    double acc_update_time = 0.0;
    int update_iterations = 0;

    //avg time of get_complete_hessian_derivs():
    double acc_gchd_time = 0.0;
    int gchd_iterations = 0;
#endif

constexpr float HECTOR_SLAM_UPDATE_FREE_FACTOR = 0.4f;
constexpr float HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR = 0.9f;

constexpr float HECTOR_SLAM_DISTANCE_THRESHOLD = 0.4f;
constexpr float HECTOR_SLAM_ANGLE_THRESHOLD = 0.9f;

constexpr int HECTOR_SLAM_ITERATIONS = 4;
constexpr int HECTOR_SLAM_ITERATIONS_FINAL = 6;

constexpr float hs_prob_to_log_odds(float prob) {
	return log(prob / (1.0f - prob));
}

constexpr float HECTOR_SLAM_MAX_MAP_VALUE = 50.0f;

constexpr float log_odds_free =
	hs_prob_to_log_odds(HECTOR_SLAM_UPDATE_FREE_FACTOR);
constexpr float log_odds_occupied =
	hs_prob_to_log_odds(HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR);

static inline vec2 hs_position_of_pose(vec3 in) {
	return vec2(in.x, in.y);
}

static inline float hs_normalize_angle_pos(float angle) {
	return fmod(fmod(angle, 2.0f*M_PI) + 2.0f*M_PI, 2.0f*M_PI);
}

static inline float hs_normalize_angle(float angle) {
	float a = hs_normalize_angle_pos(angle);
	if (a > M_PI){
		a -= 2.0f*M_PI;
	}
	return a;
}

static inline float hs_sqr(float val)
{
	return val * val;
}

static inline int hs_sign(int x)
{
	return x > 0 ? 1 : -1;
}

void hs_init(HectorSlam &slam) {
	float mapResolution = HECTOR_SLAM_MAP_CELL_LENGTH;
	int mapSizeX = HECTOR_SLAM_MAP_WIDTH;
	int mapSizeY = HECTOR_SLAM_MAP_HEIGHT;

	slam.lastUpdatePosition = vec3(FLT_MAX);
	slam.lastPosition = vec3(0.0f);

	slam.width = mapSizeX;
	slam.height = mapSizeY;

	slam.maps[0].width = mapSizeX;
	slam.maps[0].height = mapSizeY;
	slam.maps[0].cellSize = mapResolution;
	slam.maps[0].mapScale = 1.0f;

	for (size_t i = 1; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		slam.maps[i].width = slam.maps[i - 1].width / 2;
		slam.maps[i].height = slam.maps[i - 1].height / 2;
		slam.maps[i].cellSize = slam.maps[i - 1].cellSize * 2.0f;
		slam.maps[i].mapScale = slam.maps[i - 1].mapScale / 2.0f;
	}

	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		slam.maps[i].values =
			(float *)calloc(slam.maps[i].width*slam.maps[i].height,
							sizeof(float));
		slam.maps[i].updateIndex =
			(unsigned int *)calloc(slam.maps[i].width*slam.maps[i].height,
								   sizeof(unsigned int));

		slam.maps[i].currentUpdateIndex = 1;

		float scaleToMap = 1.0f / slam.maps[i].cellSize;
		vec2 topLeftOffset =
			vec2((float)slam.maps[i].width,
				 (float)slam.maps[i].height) *
			slam.maps[i].cellSize / 2.0f;

		slam.maps[i].worldToMap =
			glm::scale(mat3(1.0f), vec2(scaleToMap)) *
			glm::translate(mat3(1.0f), topLeftOffset);
		slam.maps[i].mapToWorld = glm::inverse(slam.maps[i].worldToMap);
	}
}

void hs_free(HectorSlam &slam) {
	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		free(slam.maps[i].values);
		free(slam.maps[i].updateIndex);
	}
}

void hs_clear(HectorSlam &slam) {
	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		size_t numCells = slam.maps[i].width*slam.maps[i].width;
		memset(slam.maps[i].values,      0, numCells * sizeof(float));
		memset(slam.maps[i].updateIndex, 0, numCells * sizeof(float));
		slam.maps[i].currentUpdateIndex = 1;
	}

	slam.lastPosition = vec3(0.0f);
	slam.lastUpdatePosition = vec3(FLT_MAX);
}

static inline vec3 hs_get_world_coords_pose(HectorSlamOccGrid &map,
											const vec3& mapPose) {
	vec2 worldCoords = map.mapToWorld * vec3(hs_position_of_pose(mapPose), 1.0f);
	return vec3(worldCoords[0], worldCoords[1], mapPose[2]);
}

static inline vec3 hs_get_map_coords_pose(HectorSlamOccGrid &map,
										  const vec3& worldPose) {
	vec2 mapCoords = map.worldToMap * vec3(hs_position_of_pose(worldPose), 1.0f);
	return vec3(mapCoords[0], mapCoords[1], worldPose[2]);
}


static inline unsigned int hs_curr_occ_update_index(unsigned int i) {
	return i + 2;
}

static inline unsigned int hs_curr_free_update_index(unsigned int i) {
	return i + 1;
}

static inline void hs_bresenham_cell_free(HectorSlamOccGrid &map,
										  unsigned int offset) {
	unsigned int updateIndex = map.updateIndex[offset];
	unsigned int currMarkFreeIndex = hs_curr_free_update_index(map.currentUpdateIndex);

	if (updateIndex < currMarkFreeIndex) {
		if (map.values[offset] > -(HECTOR_SLAM_MAX_MAP_VALUE + log_odds_free)) {
			map.values[offset] += log_odds_free;
		}
		map.updateIndex[offset] = currMarkFreeIndex;
	}
}

static inline void hs_bresenham_cell_occ(HectorSlamOccGrid &map,
										 unsigned int offset) {
	unsigned int updateIndex = map.updateIndex[offset];
	unsigned int currMarkOccIndex = hs_curr_occ_update_index(map.currentUpdateIndex);
	unsigned int currMarkFreeIndex = hs_curr_free_update_index(map.currentUpdateIndex);

	if (updateIndex < currMarkOccIndex) {
		//if this cell has been updated as free in the current iteration, revert this
		if (updateIndex == currMarkFreeIndex) {
			map.values[offset] -= log_odds_free;
		}

		if (map.values[offset] < (HECTOR_SLAM_MAX_MAP_VALUE - log_odds_occupied)) {
			map.values[offset] += log_odds_occupied;
		}
		map.updateIndex[offset] = currMarkOccIndex;
	}
}

static void hs_bresenham2D(HectorSlamOccGrid &map,
						   unsigned int abs_da,
						   unsigned int abs_db,
						   int error_b,
						   int offset_a,
						   int offset_b,
						   unsigned int offset) {
	hs_bresenham_cell_free(map, offset);

	unsigned int end = abs_da-1;

	for(unsigned int i = 0; i < end; ++i){
		offset += offset_a;
		error_b += abs_db;

		if((unsigned int)error_b >= abs_da){
			offset += offset_b;
			error_b -= abs_da;
		}

		hs_bresenham_cell_free(map, offset);
	}
}

static void hs_update_line_bresenhami(HectorSlamOccGrid &map,
									  const vec2i &beginMap,
									  const vec2i &endMap,
									  unsigned int max_length = UINT_MAX) {
	int x0 = beginMap[0];
	int y0 = beginMap[1];

	//check if beam start point is inside map, cancel update if this is not the case
	if ((x0 < 0) || (x0 >= (int)map.width) || (y0 < 0) || (y0 >= (int)map.height)) {
		return;
	}

	int x1 = endMap[0];
	int y1 = endMap[1];

	//check if beam end point is inside map, cancel update if this is not the case
	if ((x1 < 0) || (x1 >= (int)map.width) || (y1 < 0) || (y1 >= (int)map.height)) {
		return;
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	unsigned int abs_dx = abs(dx);
	unsigned int abs_dy = abs(dy);

	int offset_dx = hs_sign(dx);
	int offset_dy = hs_sign(dy) * (int)map.width;

	unsigned int startOffset = beginMap.y * map.width + beginMap.x;

	//if x is dominant
	if(abs_dx >= abs_dy){
		int error_y = abs_dx / 2;
		hs_bresenham2D(map, abs_dx, abs_dy, error_y, offset_dx, offset_dy, startOffset);
	}else{
		//otherwise y is dominant
		int error_x = abs_dy / 2;
		hs_bresenham2D(map, abs_dy, abs_dx, error_x, offset_dy, offset_dx, startOffset);
	}

	unsigned int endOffset = endMap.y * map.width + endMap.x;
	hs_bresenham_cell_occ(map, endOffset);
}

static void hs_update_map_by_scan(HectorSlamOccGrid &map,
								  const vec2 *const points, const size_t numPoints,
								  const vec3 &newPose) {
	//Get pose in map coordinates from pose in world coordinates
	vec3 mapPose = hs_get_map_coords_pose(map, newPose);

	//Get a 2D homogenous transform that can be left-multiplied to a
	//robot coordinates vector to get world coordinates of that vector
	mat3 poseTransform =
		glm::translate(mat3(1.0f), hs_position_of_pose(mapPose)) *
		glm::rotate(mat3(1.0f), mapPose.z);

	//Get start point of all laser beams in map coordinates (same for
	//alle beams, stored in robot coords in dataContainer)
	vec2 scanBeginMapf = hs_position_of_pose(mapPose);

	//Get integer vector of laser beams start point
	vec2i scanBeginMapi = scanBeginMapf + 0.5f;

	//Get number of valid beams in current scan
	int numValidElems = (int)numPoints;

	//Iterate over all valid laser beams
	for (int i = 0; i < numValidElems; ++i) {

		//Get map coordinates of current beam endpoint
		vec2 scanEndMapf = vec2(poseTransform * vec3(points[i] * map.mapScale, 1.0f));

		//add 0.5 to beam endpoint vector for following integer cast
		//(to round, not truncate)
		scanEndMapf += 0.5f;

		//Get integer map coordinates of current beam endpoint
		vec2i scanEndMapi = (vec2i)scanEndMapf;

		//Update map using a bresenham variant for drawing a line from
		//beam start to beam endpoint in map coordinates
		if (scanBeginMapi != scanEndMapi){
			hs_update_line_bresenhami(map, scanBeginMapi, scanEndMapi);
		}
	}

	//Increase update index (used for updating grid cells only once per incoming scan)
	map.currentUpdateIndex += 3;
}

static void hs_update_by_scan(HectorSlam &slam,
							  const vec2 *const points, const size_t numPoints,
							  vec3 newPose) {
	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {

        #ifdef PROFILING
        	timespec start;
        	timespec_get(&start, TIME_UTC);
        #endif

		hs_update_map_by_scan(slam.maps[i], points, numPoints, newPose);

        #ifdef PROFILING
        	timespec stop;
       		timespec_get(&stop, TIME_UTC);
        	acc_umapbs_time[i] += timespec_diff_to_sec(start, stop);
        	umapbs_iterations[i] += 1;
        #endif
	}
}

static float hs_get_grid_probability(float logOddsValue) {
	float odds = exp(logOddsValue);
	return odds / (odds + 1.0f);
}

static vec3 hs_interp_map_value_with_derivatives(HectorSlamOccGrid &map,
													 const vec2 &coords) {
	if ((coords[0] < 0.0f) ||
		(coords[0] > (float)(map.width - 2)) ||
		(coords[1] < 0.0f) ||
		(coords[1] > (float)(map.height - 2))) {
		return vec3(0.0f, 0.0f, 0.0f);
	}

	//map coords are always positive, floor them by casting to int
	vec2i indMin = coords;

	//get factors for bilinear interpolation
	vec2 factors(coords - (vec2)indMin);

	int index = indMin[1] * map.width + indMin[0];

	vec4 intensities;

	if (index < 0 || (index + map.width + 1) >= map.width * map.height) {
		return vec3(0.0f, 0.0f, 0.0f);
	}

	// get grid values for the 4 grid points surrounding the current
	// coords. Check cached data first, if not contained filter
	// gridPoint with gaussian and store in cache.
	intensities[0] = hs_get_grid_probability(map.values[index]);
	intensities[1] = hs_get_grid_probability(map.values[index + 1]);
	intensities[2] = hs_get_grid_probability(map.values[index + map.width]);
	intensities[3] = hs_get_grid_probability(map.values[index + map.width + 1]);

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

static inline mat3 hs_get_transform_for_state(const vec3& pose) {
	mat3 res = mat3(1.0f);
	res *= glm::translate(mat3(1.0f), hs_position_of_pose(pose));
	res *= glm::rotate(mat3(1.0f), pose.z);
	return res;
}


static void hs_get_complete_hessian_derivs(HectorSlamOccGrid &map,
										   const vec3 &pose,
										   const vec2 *const points, const size_t numPoints,
										   mat3 &H,
										   vec3 &dTr) {
	mat3 transform =
		hs_get_transform_for_state(pose);

	float sinRot = sin(pose[2]);
	float cosRot = cos(pose[2]);

	H = mat3(0.0f);
	dTr = vec3(0.0f);

	for (size_t i = 0; i < numPoints; ++i) {
		vec2 currPoint = points[i] * map.mapScale;

		vec3 transformedPointData =
			hs_interp_map_value_with_derivatives(map, vec2(transform * vec3(currPoint, 1.0f)));

		float funVal = 1.0f - transformedPointData[0];

		dTr[0] += transformedPointData[1] * funVal;
		dTr[1] += transformedPointData[2] * funVal;

		float rotDeriv =
			((-sinRot * currPoint.x - cosRot * currPoint.y) * transformedPointData[1] +
			 ( cosRot * currPoint.x - sinRot * currPoint.y) * transformedPointData[2]);

		dTr[2] += rotDeriv * funVal;

		H[0][0] += hs_sqr(transformedPointData[1]);
		H[1][1] += hs_sqr(transformedPointData[2]);
		H[2][2] += hs_sqr(rotDeriv);

		H[0][1] += transformedPointData[1] * transformedPointData[2];
		H[0][2] += transformedPointData[1] * rotDeriv;
		H[1][2] += transformedPointData[2] * rotDeriv;
	}

	H[1][0] = H[0][1];
	H[2][0] = H[0][2];
	H[2][1] = H[1][2];
}

static bool hs_estimate_transformation_log_lh(HectorSlamOccGrid &map,
											  vec3 &estimate,
											  const vec2 * const points, const size_t numPoints) {
	mat3 H;
	vec3 dTr;

    #ifdef PROFILING
        timespec start;
        timespec_get(&start, TIME_UTC);
    #endif

    hs_get_complete_hessian_derivs(map, estimate, points, numPoints, H, dTr);

    #ifdef PROFILING
        timespec stop;
        timespec_get(&stop, TIME_UTC);
        acc_gchd_time += timespec_diff_to_sec(start, stop);
        gchd_iterations += 1;
    #endif

	if ((H[0][0] != 0.0f) && (H[1][1] != 0.0f)) {
		vec3 searchDir (glm::inverse(H) * dTr);

		if (searchDir[2] > 0.2f) {
			searchDir[2] = 0.2f;
			std::cout << "SearchDir angle change too large\n";
		} else if (searchDir[2] < -0.2f) {
			searchDir[2] = -0.2f;
			std::cout << "SearchDir angle change too large\n";
		}

		if (searchDir.x != searchDir.x ||
			searchDir.y != searchDir.y ||
			searchDir.z != searchDir.z) {
			std::cout << "SearchDir is NAN!" << std::endl;
			return false;
		}

		estimate += searchDir;
		return true;
	}
	return false;
}

static vec3 hs_match_data(HectorSlamOccGrid &map,
									 const vec3 &beginEstimateWorld,
									 const vec2 *const points, const size_t numPoints,
									 int maxIterations) {

	vec3 beginEstimateMap(hs_get_map_coords_pose(map, beginEstimateWorld));
	vec3 estimate(beginEstimateMap);

	for (int i = 0; i < maxIterations; ++i) {
		hs_estimate_transformation_log_lh(map, estimate, points, numPoints);
	}

	estimate[2] = hs_normalize_angle(estimate[2]);

	return hs_get_world_coords_pose(map, estimate);
}

static bool hs_pose_difference_larger_than(const vec3& pose1,
										   const vec3& pose2,
										   float distanceDiffThresh,
										   float angleDiffThresh)
{
	//check distance
	if (glm::length(hs_position_of_pose(pose1) - hs_position_of_pose(pose2)) > distanceDiffThresh) {
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

void hs_update(HectorSlam &slam, const vec2 *const points, const size_t numPoints) {
	vec3 newPoseEstimateWorld = slam.lastPosition;

    #ifdef PROFILING
        timespec start_u;
        timespec_get(&start_u, TIME_UTC);
    #endif

	for (int i = HECTOR_SLAM_MAP_RESOLUTIONS - 1; i >= 0; --i){
        #ifdef PROFILING
        	timespec start_md;
        	timespec_get(&start_md, TIME_UTC);
        #endif

        newPoseEstimateWorld =
		    hs_match_data(slam.maps[i],
						  newPoseEstimateWorld,
						  points, numPoints,
						     (i == 0)
						   ? HECTOR_SLAM_ITERATIONS_FINAL
						   : HECTOR_SLAM_ITERATIONS);

        #ifdef PROFILING
            timespec stop_md;
            timespec_get(&stop_md, TIME_UTC);
            acc_mdata_time[i] += timespec_diff_to_sec(start_md, stop_md);
            mdata_iterations[i] += 1;
        #endif
	}

	slam.lastPosition = newPoseEstimateWorld;

	if (hs_pose_difference_larger_than(newPoseEstimateWorld,
									   slam.lastUpdatePosition,
									   HECTOR_SLAM_DISTANCE_THRESHOLD,
									   HECTOR_SLAM_ANGLE_THRESHOLD)) {
		hs_update_by_scan(slam, points, numPoints, newPoseEstimateWorld);
		slam.lastUpdatePosition = newPoseEstimateWorld;
	}
    #ifdef PROFILING
        timespec stop_u;
        timespec_get(&stop_u, TIME_UTC);
        acc_update_time += timespec_diff_to_sec(start_u, stop_u);
        update_iterations += 1;
    #endif
}

void hs_print_profiling_data(){
    #ifdef PROFILING
        printf("hs_update():\n Average time: %f \n", acc_update_time/update_iterations);

        printf("hs_match_data(): \n");
        for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
            printf("Average time of map %zu: %f \n", i, acc_mdata_time[i]/mdata_iterations[i]);
        }

        printf("update_map_by_scan(): \n");
        for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
            printf("Average time of map %zu: %f \n", i, acc_umapbs_time[i]/umapbs_iterations[i]);
        }
        printf("hs_get_complete_hessian_derivs():\n Average time: %f \n", acc_gchd_time/gchd_iterations);
    #endif
}
