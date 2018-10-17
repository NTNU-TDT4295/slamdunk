#include "slam.h"
#include <Eigen/Dense>
#include <iostream>
#include <iostream>
#include <sys/socket.h>
#include "hector_slam_lib/slam_main/HectorSlamProcessor.h"
#include "opengl.h"

static int send_point_data(int fd, vec3 point) {
	int32_t buffer[3];

	if (fd == -1) {
		return -1;
	}

	buffer[0] = (int32_t)(point.x * 1000.0f);
	buffer[1] = (int32_t)(point.y * 1000.0f);
	buffer[2] = (int32_t)(point.z * 1000.0f);

	ssize_t err;
	err = send(fd, buffer, sizeof(buffer), 0);
	if (err < 0) {
		perror("send");
		return -1;
	}

	return 0;
}

class HectorDrawings : public DrawInterface {
public:
	int socket;
	float height;

	virtual void drawPoint(const Eigen::Vector2f &pointWorldFrame) {
		// printf("point %f,%f\n", pointWorldFrame.x(), pointWorldFrame.y());

		send_point_data(socket, vec3(pointWorldFrame.x(),
									 height,
									 pointWorldFrame.y()));
	}

	virtual void drawArrow(const Eigen::Vector3f &poseWorld) {
	}

	virtual void drawCovariance(const Eigen::Vector2f &mean, const Eigen::Matrix2f &covMatrix) {
	}

	virtual void setScale(double scale) {
		// printf("set scale %f\n", scale);
	}

	virtual void setColor(double r, double g, double b, double a = 1.0) {
		// printf("set color %f %f %f %f\n", r, g, b, a);
		height = (r + b + g) / 3.0f;
	}

	virtual void sendAndResetData() {
		// printf("send and reset\n");
	}
};

class DebugInfoPrinter : public HectorDebugInfoInterface {
public:
	virtual void sendAndResetData() {
		printf("Send and reset data\n");
	}

	virtual void addHessianMatrix(const Eigen::Matrix3f &hessian) {
		printf("Add hessian matrix\n");
	}

	virtual void addPoseLikelihood(float lh) {
		printf("Add pose likelihood\n");
	}
};

struct InternalSlamContext {
	hectorslam::HectorSlamProcessor *processor;
	hectorslam::DataContainer container;
	HectorDrawings drawings;
};

void init_slam(SlamContext &ctx) {
	float mapResolution = 0.025;
	int mapSizeX = 1024;
	int mapSizeY = 1024;
	Eigen::Vector2f startCoord = { 0.5f, 0.5f };
	int mapDepth = 3;

	ctx.internal = new InternalSlamContext();

	ctx.internal->processor =
		new hectorslam::HectorSlamProcessor(mapResolution,
											mapSizeX, mapSizeY,
											startCoord,
											mapDepth);
											// &ctx.internal->drawings);

	ctx.internal->processor->setUpdateFactorFree(0.4f);
	ctx.internal->processor->setUpdateFactorOccupied(0.9f);
	ctx.internal->processor->setMapUpdateMinDistDiff(0.4f);
	ctx.internal->processor->setMapUpdateMinAngleDiff(0.9f);

	ctx.internal->container.setOrigo(Eigen::Vector2f::Zero());

	init_lidar_socket(ctx.lidar_socket, "0.0.0.0", "6002");

	ctx.internal->drawings.socket = net_client_connect("127.0.0.1", "6000");

	// glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void tick_slam(SlamContext &ctx, const WindowFrameInfo &info) {
	sem_wait(&ctx.lidar_socket.lock);

	size_t length = ctx.lidar_socket.scan_data_length[ctx.lidar_socket.scan_data_read_select];
	// printf("%zu\n", length);

	ctx.internal->container.clear();

	float scale = ctx.internal->processor->getScaleToMap();

	for (size_t i = 0; i < length; i++) {
		vec2 p = ctx.lidar_socket.scan_data[ctx.lidar_socket.scan_data_read_select][i];
		float angle = p.x * M_PI / 180.0f;
		float dist  = (p.y / 1000.0f) * scale;

		if (dist > 0.1f) {
			Eigen::Vector2f pos = Eigen::Vector2f(cos(angle) * dist, sin(angle) * dist);
			// printf("%f %f\n", pos.x(), pos.y());
			ctx.internal->container.add(pos);
		}
	}

	sem_post(&ctx.lidar_socket.lock);

	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ctx.internal->container.getSize() > 0) {
		ctx.internal->processor->update(ctx.internal->container,
										ctx.internal->processor->getLastScanMatchPose());
	}

	const hectorslam::GridMap &map = ctx.internal->processor->getGridMap();

	size_t mapWidth  = map.getSizeX();
	size_t mapHeight = map.getSizeY();

	for (size_t i = 0; i < mapWidth * mapHeight; i++) {
		if (map.isOccupied(i)) {
			size_t x = i % mapWidth;
			size_t y = i / mapWidth;

			vec3 p = {
				(float)x - ((float)mapWidth / 2.0f),
				0.0f,
				(float)y - ((float)mapHeight / 2.0f)
			};

			// printf("send point %f %f\n", p.x, p.y);
			send_point_data(ctx.internal->drawings.socket, p);
		}
	}

	// Eigen::Vector3f pose = ctx.internal->processor->getLastScanMatchPose();
	//printf("%i %f %f %f\n", ctx.internal->container.getSize(), pose.x(), pose.y(), pose.z());
}

void free_slam(SlamContext &ctx) {
	free_lidar_socket(ctx.lidar_socket);
	delete ctx.internal->processor;
	delete ctx.internal;
}
