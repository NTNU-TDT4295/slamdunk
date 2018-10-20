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
	//hectorslam::HectorSlamProcessor *proc;
	hectorslam::DataContainer *container;
	hectorslam::MapRepMultiMap *mapRep;

	float mapResolution = 0.025;
	int mapSizeX = 1024;
	int mapSizeY = 1024;
	Eigen::Vector2f startCoord = { 0.5f, 0.5f };
	int mapDepth = 3;

	// proc = new hectorslam::HectorSlamProcessor(mapResolution,
	// 										   mapSizeX, mapSizeY,
	// 										   startCoord,
	// 										   mapDepth);

	// proc->setUpdateFactorFree(HECTOR_SLAM_UPDATE_FREE_FACTOR);
	// proc->setUpdateFactorOccupied(HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR);
	// proc->setMapUpdateMinDistDiff(HECTOR_SLAM_DISTANCE_THRESHOLD);
	// proc->setMapUpdateMinAngleDiff(HECTOR_SLAM_ANGLE_THRESHOLD);

    mapRep = new hectorslam::MapRepMultiMap(mapResolution, mapSizeX, mapSizeY, mapDepth, startCoord, NULL, NULL);

    slam.lastUpdatePosition = vec3(FLT_MAX);
    slam.lastPosition = vec3(0.0f);
    mapRep->reset();

	slam.width = mapSizeX;
	slam.height = mapSizeY;

	slam.maps[0].width = mapSizeX;
	slam.maps[0].height = mapSizeY;
	slam.maps[0].values = (float *)calloc(mapSizeX*mapSizeY, sizeof(float));
	slam.maps[0].cellSize = mapResolution;


	container = new hectorslam::DataContainer();
	container->setOrigo(Eigen::Vector2f::Zero());

	// slam.proc = proc;
	slam.mapRep = mapRep;
	slam.cont = container;
}

void hs_free(HectorSlam &slam) {
}

void hs_update(HectorSlam &slam, vec2 *points, size_t numPoints) {
	// hectorslam::HectorSlamProcessor *proc;
	hectorslam::DataContainer *container;
	hectorslam::MapRepMultiMap *mapRep;

	// proc = (hectorslam::HectorSlamProcessor *)slam.proc;
	container = (hectorslam::DataContainer *)slam.cont;
	mapRep = (hectorslam::MapRepMultiMap *)slam.mapRep;

	container->clear();

	for (size_t i = 0; i < numPoints; i++) {
		container->add(Eigen::Vector2f(points[i].x, points[i].y));
	}



	// proc->update(*container, proc->getLastScanMatchPose());



	Eigen::Matrix3f cov;
    Eigen::Vector3f newPoseEstimateWorld;
	newPoseEstimateWorld = (mapRep->matchData(to_eigen(slam.lastPosition), *container, cov));

    slam.lastPosition = to_glm(newPoseEstimateWorld);

	if (util::poseDifferenceLargerThan(newPoseEstimateWorld,
									   to_eigen(slam.lastUpdatePosition),
									   HECTOR_SLAM_DISTANCE_THRESHOLD,
									   HECTOR_SLAM_ANGLE_THRESHOLD)) {
		mapRep->updateByScan(*container, newPoseEstimateWorld);
		mapRep->onMapUpdated();
		slam.lastUpdatePosition = to_glm(newPoseEstimateWorld);
	}













	// slam.lastPosition = to_glm(proc->getLastScanMatchPose());

	const hectorslam::GridMap &map = mapRep->getGridMap(0);

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
