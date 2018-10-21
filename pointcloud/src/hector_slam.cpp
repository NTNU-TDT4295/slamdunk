#include "hector_slam.h"
#include <Eigen/Dense>
#include <iostream>
#include "hector_slam_lib/scan/DataPointContainer.h"

#include <float.h>

constexpr float hs_prob_to_log_odds(float prob) {
	return log(prob / (1.0f - prob));
}

constexpr float log_odds_free =
	hs_prob_to_log_odds(HECTOR_SLAM_UPDATE_FREE_FACTOR);
constexpr float log_odds_occupied =
	hs_prob_to_log_odds(HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR);

static inline vec3 to_glm(const Eigen::Vector3f &in) {
	return vec3(in.x(), in.y(), in.z());
}

static inline Eigen::Vector3f to_eigen(vec3 in) {
	return Eigen::Vector3f(in.x, in.y, in.z);
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
	hectorslam::DataContainer *container;

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

	for (size_t i = 1; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		slam.maps[i].width = slam.maps[i - 1].width / 2;
		slam.maps[i].height = slam.maps[i - 1].height / 2;
		slam.maps[i].cellSize = slam.maps[i - 1].cellSize * 2.0f;
	}

	// float totalMapSizeX = mapResolution * static_cast<float>(mapSizeX);
	// float midOffsetX = totalMapSizeX * startCoord.x();

	// float totalMapSizeY = mapResolution * static_cast<float>(mapSizeY);
	// float midOffsetY = totalMapSizeY * startCoord.y();

	// Eigen::Vector2i resolution = Eigen::Vector2i(slam.width, slam.height);
	// Eigen::Vector2f midOffset = Eigen::Vector2f(midOffsetX, midOffsetY);

	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		slam.maps[i].values =
			(float *)calloc(slam.maps[i].width*slam.maps[i].height,
							sizeof(float));
		slam.maps[i].updateIndex =
			(unsigned int *)calloc(slam.maps[i].width*slam.maps[i].height,
								   sizeof(unsigned int));

		slam.maps[i].currentUpdateIndex = 1;
	}


	container = new hectorslam::DataContainer();
	container->setOrigo(Eigen::Vector2f::Zero());

	slam.cont = container;
}

void hs_free(HectorSlam &slam) {
}

static inline Eigen::Vector3f hs_get_world_coords_pose(HectorSlamOccGrid &map,
													   const Eigen::Vector3f& mapPose) {
	float scaleToMap = 1.0f / map.cellSize;
	Eigen::Vector2f topLeftOffset =
		Eigen::Vector2f((float)map.width, (float)map.height) * map.cellSize / 2.0f;

	Eigen::Affine2f mapTworld =
		Eigen::AlignedScaling2f(scaleToMap, scaleToMap) *
		Eigen::Translation2f(topLeftOffset[0], topLeftOffset[1]);
	Eigen::Affine2f worldTmap = mapTworld.inverse();

	Eigen::Vector2f worldCoords (worldTmap * mapPose.head<2>());
	return Eigen::Vector3f(worldCoords[0], worldCoords[1], mapPose[2]);
}

static inline Eigen::Vector3f hs_get_map_coords_pose(HectorSlamOccGrid &map,
													 const Eigen::Vector3f& worldPose) {
	float scaleToMap = 1.0f / map.cellSize;
	Eigen::Vector2f topLeftOffset =
		Eigen::Vector2f((float)map.width, (float)map.height) * map.cellSize / 2.0f;

	Eigen::Affine2f mapTworld =
		Eigen::AlignedScaling2f(scaleToMap, scaleToMap) *
		Eigen::Translation2f(topLeftOffset[0], topLeftOffset[1]);

	Eigen::Vector2f mapCoords (mapTworld * worldPose.head<2>());
	return Eigen::Vector3f(mapCoords[0], mapCoords[1], worldPose[2]);
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
		map.values[offset] += log_odds_free;

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

		map.values[offset] += log_odds_occupied;
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
									  const Eigen::Vector2i &beginMap,
									  const Eigen::Vector2i &endMap,
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

	unsigned int startOffset = beginMap.y() * map.width + beginMap.x();

	//if x is dominant
	if(abs_dx >= abs_dy){
		int error_y = abs_dx / 2;
		hs_bresenham2D(map, abs_dx, abs_dy, error_y, offset_dx, offset_dy, startOffset);
	}else{
		//otherwise y is dominant
		int error_x = abs_dy / 2;
		hs_bresenham2D(map, abs_dy, abs_dx, error_x, offset_dy, offset_dx, startOffset);
	}

	unsigned int endOffset = endMap.y() * map.width + endMap.x();
	hs_bresenham_cell_occ(map, endOffset);
}

static void hs_update_map_by_scan(HectorSlamOccGrid &map,
								  const hectorslam::DataContainer &points,
								  const Eigen::Vector3f &newPose) {
	// currMarkFreeIndex = currUpdateIndex + 1;
	// currMarkOccIndex = currUpdateIndex + 2;

	//Get pose in map coordinates from pose in world coordinates
	Eigen::Vector3f mapPose(hs_get_map_coords_pose(map, newPose));

	//Get a 2D homogenous transform that can be left-multiplied to a robot coordinates vector to get world coordinates of that vector
	Eigen::Affine2f poseTransform((Eigen::Translation2f(mapPose[0], mapPose[1]) *
								   Eigen::Rotation2Df(mapPose[2])));

	//Get start point of all laser beams in map coordinates (same for alle beams, stored in robot coords in dataContainer)
	Eigen::Vector2f scanBeginMapf(poseTransform * points.getOrigo());

	//Get integer vector of laser beams start point
	Eigen::Vector2i scanBeginMapi(scanBeginMapf[0] + 0.5f, scanBeginMapf[1] + 0.5f);

	//Get number of valid beams in current scan
	int numValidElems = points.getSize();

	//Iterate over all valid laser beams
	for (int i = 0; i < numValidElems; ++i) {

		//Get map coordinates of current beam endpoint
		Eigen::Vector2f scanEndMapf(poseTransform * (points.getVecEntry(i)));

		//add 0.5 to beam endpoint vector for following integer cast (to round, not truncate)
		scanEndMapf.array() += (0.5f);

		//Get integer map coordinates of current beam endpoint
		Eigen::Vector2i scanEndMapi(scanEndMapf.cast<int>());

		//Update map using a bresenham variant for drawing a line from beam start to beam endpoint in map coordinates
		if (scanBeginMapi != scanEndMapi){
			hs_update_line_bresenhami(map, scanBeginMapi, scanEndMapi);
		}
	}

	//Increase update index (used for updating grid cells only once per incoming scan)
	map.currentUpdateIndex += 3;
}

static void hs_update_by_scan(HectorSlam &slam, hectorslam::DataContainer &points, Eigen::Vector3f newPose) {
	hectorslam::DataContainer tmpDataContainer;

	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		if (i == 0) {
			hs_update_map_by_scan(slam.maps[i], points, newPose);
		} else {
			float scale = static_cast<float>(1.0 / pow(2.0, static_cast<double>(i)));
			tmpDataContainer.setFrom(points, scale);
			hs_update_map_by_scan(slam.maps[i], tmpDataContainer, newPose);
		}
	}
}

float hs_get_grid_probability(float logOddsValue) {
    float odds = exp(logOddsValue);
    return odds / (odds + 1.0f);
}

Eigen::Vector3f hs_interp_map_value_with_derivatives(HectorSlamOccGrid &map,
													 const Eigen::Vector2f &coords) {
	if ((coords[0] < 0.0f) ||
		(coords[0] > (float)(map.width - 2)) ||
		(coords[1] < 0.0f) ||
		(coords[1] > (float)(map.height - 2))) {
		return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	}

	//map coords are always positive, floor them by casting to int
	Eigen::Vector2i indMin(coords.cast<int>());

	//get factors for bilinear interpolation
	Eigen::Vector2f factors(coords - indMin.cast<float>());

	int sizeX = map.width;

	int index = indMin[1] * sizeX + indMin[0];

	Eigen::Vector4f intensities;

	// get grid values for the 4 grid points surrounding the current
	// coords. Check cached data first, if not contained filter
	// gridPoint with gaussian and store in cache.
	intensities[0] = hs_get_grid_probability(map.values[index]);

	++index;

	intensities[1] = hs_get_grid_probability(map.values[index]);

	index += sizeX-1;

	intensities[2] = hs_get_grid_probability(map.values[index]);

	++index;

	intensities[3] = hs_get_grid_probability(map.values[index]);

	float dx1 = intensities[0] - intensities[1];
	float dx2 = intensities[2] - intensities[3];

	float dy1 = intensities[0] - intensities[2];
	float dy2 = intensities[1] - intensities[3];

	float xFacInv = (1.0f - factors[0]);
	float yFacInv = (1.0f - factors[1]);

	return Eigen::Vector3f(
						   ((intensities[0] * xFacInv + intensities[1] * factors[0]) * (yFacInv)) +
						   ((intensities[2] * xFacInv + intensities[3] * factors[0]) * (factors[1])),
						   -((dx1 * xFacInv) + (dx2 * factors[0])),
						   -((dy1 * yFacInv) + (dy2 * factors[1]))
						   );
}

static inline Eigen::Affine2f hs_get_transform_for_state(const Eigen::Vector3f& transVector) {
	return Eigen::Translation2f(transVector[0], transVector[1]) * Eigen::Rotation2Df(transVector[2]);
}


static void hs_get_complete_hessian_derivs(HectorSlamOccGrid &map,
										   const Eigen::Vector3f &pose,
										   const hectorslam::DataContainer &dataPoints,
										   Eigen::Matrix3f &H,
										   Eigen::Vector3f &dTr) {
	int size = dataPoints.getSize();

	Eigen::Affine2f transform(hs_get_transform_for_state(pose));

	float sinRot = sin(pose[2]);
	float cosRot = cos(pose[2]);

	H = Eigen::Matrix3f::Zero();
	dTr = Eigen::Vector3f::Zero();

	for (int i = 0; i < size; ++i) {

		const Eigen::Vector2f& currPoint (dataPoints.getVecEntry(i));

		Eigen::Vector3f transformedPointData(hs_interp_map_value_with_derivatives(map, transform * currPoint));

		float funVal = 1.0f - transformedPointData[0];

		dTr[0] += transformedPointData[1] * funVal;
		dTr[1] += transformedPointData[2] * funVal;

		float rotDeriv = ((-sinRot * currPoint.x() - cosRot * currPoint.y()) * transformedPointData[1] + (cosRot * currPoint.x() - sinRot * currPoint.y()) * transformedPointData[2]);

		dTr[2] += rotDeriv * funVal;

		H(0, 0) += hs_sqr(transformedPointData[1]);
		H(1, 1) += hs_sqr(transformedPointData[2]);
		H(2, 2) += hs_sqr(rotDeriv);

		H(0, 1) += transformedPointData[1] * transformedPointData[2];
		H(0, 2) += transformedPointData[1] * rotDeriv;
		H(1, 2) += transformedPointData[2] * rotDeriv;
	}

	H(1, 0) = H(0, 1);
	H(2, 0) = H(0, 2);
	H(2, 1) = H(1, 2);
}

static bool hs_estimate_transformation_log_lh(HectorSlamOccGrid &map,
											  Eigen::Vector3f &estimate,
											  const hectorslam::DataContainer &dataPoints) {
	Eigen::Matrix3f H;
	Eigen::Vector3f dTr;

	hs_get_complete_hessian_derivs(map, estimate, dataPoints, H, dTr);

	if ((H(0, 0) != 0.0f) && (H(1, 1) != 0.0f)) {
		Eigen::Vector3f searchDir (H.inverse() * dTr);

		if (searchDir[2] > 0.2f) {
			searchDir[2] = 0.2f;
			std::cout << "SearchDir angle change too large\n";
		} else if (searchDir[2] < -0.2f) {
			searchDir[2] = -0.2f;
			std::cout << "SearchDir angle change too large\n";
		}

		estimate += searchDir;
		return true;
	}
	return false;
}

static Eigen::Vector3f hs_match_data(HectorSlamOccGrid &map,
									 const Eigen::Vector3f &beginEstimateWorld,
									 const hectorslam::DataContainer &dataContainer,
									 int maxIterations) {

	Eigen::Vector3f beginEstimateMap(hs_get_map_coords_pose(map, beginEstimateWorld));
	Eigen::Vector3f estimate(beginEstimateMap);
	Eigen::Matrix3f covMatrix;

	for (int i = 0; i < maxIterations + 1; ++i) {
		hs_estimate_transformation_log_lh(map, estimate, dataContainer);
	}

	estimate[2] = hs_normalize_angle(estimate[2]);

	return hs_get_world_coords_pose(map, estimate);
}

static bool hs_pose_difference_larger_than(const Eigen::Vector3f& pose1,
										   const Eigen::Vector3f& pose2,
										   float distanceDiffThresh,
										   float angleDiffThresh)
{
  //check distance
  if ( ( (pose1.head<2>() - pose2.head<2>()).norm() ) > distanceDiffThresh){
    return true;
  }

  float angleDiff = (pose1.z() - pose2.z());

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

void hs_update(HectorSlam &slam, vec2 *points, size_t numPoints) {
	hectorslam::DataContainer *container;

	container = (hectorslam::DataContainer *)slam.cont;

	container->clear();

	for (size_t i = 0; i < numPoints; i++) {
		container->add(Eigen::Vector2f(points[i].x, points[i].y));
	}





	Eigen::Matrix3f covMatrix;
	Eigen::Vector3f newPoseEstimateWorld = to_eigen(slam.lastPosition);
	hectorslam::DataContainer tmpDataContainer;

	for (int i = HECTOR_SLAM_MAP_RESOLUTIONS - 1; i >= 0; --i){
		if (i == 0){
			newPoseEstimateWorld =
				hs_match_data(slam.maps[i],
							  newPoseEstimateWorld,
							  *container, 5);
		} else {
			float scale = static_cast<float>(1.0 / pow(2.0, static_cast<double>(i)));
			tmpDataContainer.setFrom(*container, scale);
			newPoseEstimateWorld =
				hs_match_data(slam.maps[i],
							  newPoseEstimateWorld,
							  tmpDataContainer, 3);
		}
	}

	slam.lastPosition = to_glm(newPoseEstimateWorld);

	if (hs_pose_difference_larger_than(newPoseEstimateWorld,
									   to_eigen(slam.lastUpdatePosition),
									   HECTOR_SLAM_DISTANCE_THRESHOLD,
									   HECTOR_SLAM_ANGLE_THRESHOLD)) {
		hs_update_by_scan(slam, *container, newPoseEstimateWorld);
		slam.lastUpdatePosition = to_glm(newPoseEstimateWorld);
	}
}
