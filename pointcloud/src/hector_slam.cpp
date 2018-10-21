#include "hector_slam.h"
#include <Eigen/Dense>
#include <iostream>
#include "hector_slam_lib/slam_main/HectorSlamProcessor.h"

static inline vec3 to_glm(const Eigen::Vector3f &in) {
	return vec3(in.x(), in.y(), in.z());
}

static inline Eigen::Vector3f to_eigen(vec3 in) {
	return Eigen::Vector3f(in.x, in.y, in.z);
}


void hs_init(HectorSlam &slam) {
	hectorslam::DataContainer *container;

	float mapResolution = 0.025;
	int mapSizeX = 1024;
	int mapSizeY = 1024;
	Eigen::Vector2f startCoord = { 0.5f, 0.5f };

	slam.lastUpdatePosition = vec3(FLT_MAX);
	slam.lastPosition = vec3(0.0f);

	slam.width = mapSizeX;
	slam.height = mapSizeY;

	slam.maps[0].width = mapSizeX;
	slam.maps[0].height = mapSizeY;
	slam.maps[0].values = (float *)calloc(mapSizeX*mapSizeY, sizeof(float));
	slam.maps[0].cellSize = mapResolution;

	for (size_t i = 1; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		slam.maps[i].width = slam.maps[i - 1].width / 2;
		slam.maps[i].height = slam.maps[i - 1].height / 2;
		slam.maps[i].cellSize = slam.maps[i - 1].cellSize * 2.0f;
	}

	float totalMapSizeX = mapResolution * static_cast<float>(mapSizeX);
	float midOffsetX = totalMapSizeX * startCoord.x();

	float totalMapSizeY = mapResolution * static_cast<float>(mapSizeY);
	float midOffsetY = totalMapSizeY * startCoord.y();

	Eigen::Vector2i resolution = Eigen::Vector2i(slam.width, slam.height);
	Eigen::Vector2f midOffset = Eigen::Vector2f(midOffsetX, midOffsetY);

	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		slam.maps[i].values = (float *)calloc(slam.maps[i].width*slam.maps[i].height, sizeof(float));

		hectorslam::GridMap* gridMap =
			new hectorslam::GridMap(slam.maps[i].cellSize, resolution, midOffset);

		gridMap->setUpdateOccupiedFactor(HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR);
		gridMap->setUpdateFreeFactor(HECTOR_SLAM_UPDATE_FREE_FACTOR);

		slam.maps[i].gridMap = gridMap;
	}


	container = new hectorslam::DataContainer();
	container->setOrigo(Eigen::Vector2f::Zero());

	slam.cont = container;
}

void hs_free(HectorSlam &slam) {
}

static void hs_update_by_scan(HectorSlam &slam, hectorslam::DataContainer &points, Eigen::Vector3f newPose) {
	hectorslam::DataContainer tmpDataContainer;

	for (size_t i = 0; i < HECTOR_SLAM_MAP_RESOLUTIONS; i++) {
		hectorslam::GridMap *gridMap =
			(hectorslam::GridMap *)slam.maps[i].gridMap;

		if (i == 0) {
			gridMap->updateByScan(points, newPose);
		} else {
			float scale = static_cast<float>(1.0 / pow(2.0, static_cast<double>(i)));
			tmpDataContainer.setFrom(points, scale);
			gridMap->updateByScan(tmpDataContainer, newPose);
		}
	}
}

Eigen::Vector3f hs_interp_map_value_with_derivatives(HectorSlamOccGrid &map,
													 const Eigen::Vector2f &coords) {
	hectorslam::GridMap *concreteGridMap =
		(hectorslam::GridMap *)map.gridMap;

	if (concreteGridMap->pointOutOfMapBounds(coords)){
		return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	}

	//map coords are always positive, floor them by casting to int
	Eigen::Vector2i indMin(coords.cast<int>());

	//get factors for bilinear interpolation
	Eigen::Vector2f factors(coords - indMin.cast<float>());

	int sizeX = concreteGridMap->getSizeX();

	int index = indMin[1] * sizeX + indMin[0];

	Eigen::Vector4f intensities;

	// get grid values for the 4 grid points surrounding the current
	// coords. Check cached data first, if not contained filter
	// gridPoint with gaussian and store in cache.
	intensities[0] = concreteGridMap->getGridProbabilityMap(index);

	++index;

	intensities[1] = concreteGridMap->getGridProbabilityMap(index);

	index += sizeX-1;

	intensities[2] = concreteGridMap->getGridProbabilityMap(index);

	++index;

	intensities[3] = concreteGridMap->getGridProbabilityMap(index);

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

static inline Eigen::Vector3f hs_get_world_coords_pose(HectorSlamOccGrid &map,
													   const Eigen::Vector3f& mapPose) {
	hectorslam::GridMap *concreteGridMap =
		(hectorslam::GridMap *)map.gridMap;
	return concreteGridMap->getWorldCoordsPose(mapPose);
}
static inline Eigen::Vector3f hs_get_map_coords_pose(HectorSlamOccGrid &map,
													 const Eigen::Vector3f& worldPose) {
	hectorslam::GridMap *concreteGridMap =
		(hectorslam::GridMap *)map.gridMap;
	return concreteGridMap->getMapCoordsPose(worldPose);
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

		H(0, 0) += util::sqr(transformedPointData[1]);
		H(1, 1) += util::sqr(transformedPointData[2]);
		H(2, 2) += util::sqr(rotDeriv);

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

	estimate[2] = util::normalize_angle(estimate[2]);

	return hs_get_world_coords_pose(map, estimate);
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

	if (util::poseDifferenceLargerThan(newPoseEstimateWorld,
									   to_eigen(slam.lastUpdatePosition),
									   HECTOR_SLAM_DISTANCE_THRESHOLD,
									   HECTOR_SLAM_ANGLE_THRESHOLD)) {
		hs_update_by_scan(slam, *container, newPoseEstimateWorld);
		slam.lastUpdatePosition = to_glm(newPoseEstimateWorld);
	}













	const hectorslam::GridMap &map = *(hectorslam::GridMap *)slam.maps[0].gridMap;

	for (size_t i = 0; i < slam.maps[0].height * slam.maps[0].width; i++) {
		if (map.isOccupied(i)) {
			slam.maps[0].values[i] = 1.0f;
		} else if (map.isFree(i)) {
			slam.maps[0].values[i] = -1.0f;
		} else {
			slam.maps[0].values[i] = 0.0f;
		}
	}
}
