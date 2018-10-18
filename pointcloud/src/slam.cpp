#include "slam.h"
#include <Eigen/Dense>
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include "hector_slam_lib/slam_main/HectorSlamProcessor.h"
#include "opengl.h"
#include "utils.h"

#include <glm/gtc/type_ptr.hpp>

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



	const float quad[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,	1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,	1.0f, 1.0f,
	};

	GLuint vao, buffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quad),
				 &quad, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ctx.quad_vao = vao;

	glGenTextures(1, &ctx.texture);
	glBindTexture(GL_TEXTURE_2D, ctx.texture);

	ctx.tex_buffer = (uint8_t *)calloc(mapSizeX * mapSizeY, sizeof(uint8_t));

	memset(ctx.tex_buffer, 0, mapSizeX * mapSizeY * sizeof(uint8_t));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mapSizeX, mapSizeY, 0,
				 GL_RED, GL_UNSIGNED_BYTE, ctx.tex_buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint vshader, fshader;
	vshader = create_shader_from_file("assets/shaders/test.vsh", GL_VERTEX_SHADER);
	fshader = create_shader_from_file("assets/shaders/texture.fsh", GL_FRAGMENT_SHADER);

	ctx.shader.id = glCreateProgram();

	glAttachShader(ctx.shader.id, vshader);
	glAttachShader(ctx.shader.id, fshader);

	if (!link_shader_program(ctx.shader.id)) {
		panic("Could not compile the shader!");
	}

	ctx.shader.in_matrix            = glGetUniformLocation(ctx.shader.id, "in_matrix");
	ctx.shader.in_projection_matrix = glGetUniformLocation(ctx.shader.id, "in_projection_matrix");
	ctx.shader.in_tex = glGetUniformLocation(ctx.shader.id, "tex");

	glUseProgram(ctx.shader.id);

	mat4 mat (1.0f);

	glUniformMatrix4fv(ctx.shader.in_projection_matrix, 1, GL_FALSE, glm::value_ptr(mat));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(mat));
	glUniform1i(ctx.shader.in_tex, 0);

	glUseProgram(0);

	init_lidar_socket(ctx.lidar_socket, "0.0.0.0", "6002");

	ctx.internal->drawings.socket = net_client_connect("127.0.0.1", "6000");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ctx.internal->container.getSize() > 0) {
		ctx.internal->processor->update(ctx.internal->container,
										ctx.internal->processor->getLastScanMatchPose());
	}


	const hectorslam::GridMap &map = ctx.internal->processor->getGridMap();

	size_t mapWidth  = map.getSizeX();
	size_t mapHeight = map.getSizeY();

	for (size_t i = 0; i < mapWidth * mapHeight; i++) {
		// ctx.tex_buffer[i] = (uint8_t)(map.getCell(i).getValue() * 255.0f);
		if (map.isOccupied(i)) {
			ctx.tex_buffer[i] = 255;
		} else if (map.isFree(i)) {
			ctx.tex_buffer[i] = 50;
		}
		// if (map.isOccupied(i)) {
		// 	size_t x = i % mapWidth;
		// 	size_t y = i / mapWidth;

		// 	vec3 p = {
		// 		(float)x - ((float)mapWidth / 2.0f),
		// 		0.0f,
		// 		(float)y - ((float)mapHeight / 2.0f)
		// 	};

		// 	// printf("send point %f %f\n", p.x, p.y);
		// 	send_point_data(ctx.internal->drawings.socket, p);
		// }
	}

	glUseProgram(ctx.shader.id);
	glBindVertexArray(ctx.quad_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx.texture);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mapWidth, mapHeight,
					GL_RED, GL_UNSIGNED_BYTE, ctx.tex_buffer);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	// Eigen::Vector3f pose = ctx.internal->processor->getLastScanMatchPose();
	//printf("%i %f %f %f\n", ctx.internal->container.getSize(), pose.x(), pose.y(), pose.z());
}

void free_slam(SlamContext &ctx) {
	free_lidar_socket(ctx.lidar_socket);
	delete ctx.internal->processor;
	delete ctx.internal;
}


struct HectorSlamOccGrid {
	float *values;
	unsigned int *updateIndex;
	size_t width;
	size_t height;
	float cellSize;
	float scaleFactor;

	Eigen::Affine2f mapTworld;
};

constexpr size_t HECTOR_SLAM_MAP_RESOLUTIONS = 3;

struct HectorSlam {
	size_t width;
	size_t height;
	unsigned int updateIndex;

	HectorSlamOccGrid maps[HECTOR_SLAM_MAP_RESOLUTIONS];

	Eigen::Vector3f lastPosition;
};

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

float hs_log_odds(float val) {
	float odds = exp(val);
	return odds / (odds + 1.0f);
}

Eigen::Vector3f hs_sample_map_value_with_derivatives(const HectorSlamOccGrid &map,
													 const Eigen::Vector2f& coords) {
	// if out of bonuds
    Eigen::Vector2i indMin(coords.cast<int>());

	if (coords.x() < 0 || coords.x() >= map.width ||
		coords.y() < 0 || coords.y() >= map.height) {
		return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	}

	Eigen::Vector2f factors(coords - indMin.cast<float>());

	int sizeX = map.width;
	int index = indMin.x() * sizeX + indMin.y();

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

    return Eigen::Vector3f(
      ((intensities[0] * xFacInv + intensities[1] * factors[0]) * (yFacInv)) +
      ((intensities[2] * xFacInv + intensities[3] * factors[0]) * (factors[1])),
      -((dx1 * xFacInv) + (dx2 * factors[0])),
      -((dy1 * yFacInv) + (dy2 * factors[1]))
    );
}

Eigen::Affine2f hs_transform_from_pose(const Eigen::Vector3f &pose) {
    return Eigen::Translation2f(pose[0], pose[1]) * Eigen::Rotation2Df(pose[2]);
}

struct HessianDerivs {
	Eigen::Matrix3f H;
	Eigen::Vector3f dTr;
};

HessianDerivs hs_get_complete_hessian_derivs(const HectorSlamOccGrid &map, const Eigen::Vector3f &pose, Eigen::Vector2f *points, size_t numPoints) {
	HessianDerivs result = {0};
	result.H = Eigen::Matrix3f::Zero();
	result.dTr = Eigen::Vector3f::Zero();

    Eigen::Affine2f transform = hs_transform_from_pose(pose);

    float sinRot = sin(pose[2]);
    float cosRot = cos(pose[2]);

	for (size_t i = 0; i < numPoints; i++) {
		Eigen::Vector2f point = points[i] * map.scaleFactor;

		Eigen::Vector3f pointData =
			hs_sample_map_value_with_derivatives(map, transform * point);

		float funVal = 1.0f - pointData[0];

		result.dTr[0] += pointData[1] * funVal;
		result.dTr[1] += pointData[2] * funVal;

		float rotDeriv =
			((-sinRot * point.x() - cosRot * point.y()) * pointData[1] +
			 ( cosRot * point.x() - sinRot * point.y()) * pointData[2]);

		result.dTr[2] += rotDeriv * funVal;

		result.H(0, 0) += pointData[1] * pointData[1];
		result.H(1, 1) += pointData[2] * pointData[2];
		result.H(2, 2) += rotDeriv * rotDeriv;

		result.H(0, 1) += pointData[1] * pointData[2];
		result.H(0, 2) += pointData[1] * rotDeriv;
		result.H(1, 2) += pointData[2] * rotDeriv;
	}

	return result;
}

bool hs_estimate_transformation_log_lh(Eigen::Vector3f &estimate,
									   const HectorSlamOccGrid &map,
									   Eigen::Vector2f *points, size_t numPoints) {
	HessianDerivs hessian =
		hs_get_complete_hessian_derivs(map, estimate, points, numPoints);

	if ((hessian.H(0, 0) != 0.0f) && (hessian.H(1, 1) != 0.0f)) {
		Eigen::Vector3f searchDir (hessian.H.inverse() * hessian.dTr);

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

Eigen::Vector3f hs_map_coords_pose(const HectorSlamOccGrid &map, const Eigen::Vector3f &worldPose) {
    Eigen::Vector2f mapCoords (map.mapTworld * worldPose.head<2>());
    return Eigen::Vector3f(mapCoords[0], mapCoords[1], worldPose[2]);
}

Eigen::Vector3f hs_world_coords_pose(const HectorSlamOccGrid &map, const Eigen::Vector3f &mapPose) {
    Eigen::Vector2f worldCoords (map.mapTworld.inverse() * mapPose.head<2>());
    return Eigen::Vector3f(worldCoords[0], worldCoords[1], mapPose[2]);
}

Eigen::Vector3f hs_match_data(const HectorSlamOccGrid &map,
				   const Eigen::Vector3f &beginEstimateWorld,
				   int numIter,
				   Eigen::Vector2f *points, size_t numPoints) {
	Eigen::Vector3f estimate = hs_map_coords_pose(map, beginEstimateWorld);

	for (int i = 0; i < numIter; i++) {
		hs_estimate_transformation_log_lh(estimate, map, points, numPoints);
	}

	estimate[2] = normalize_angle(estimate[2]);

	return hs_world_coords_pose(map, estimate);
}

void hs_init(HectorSlam &slam) {
	slam.width = 1024;
	slam.height = 1024;

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
		slam.maps[i].values = (float *)calloc(numPixels, sizeof(float));
		slam.maps[i].updateIndex = (unsigned int *)calloc(numPixels, sizeof(unsigned int));

		float scaleToMap = 1.0f / slam.maps[i].cellSize;

		slam.maps[i].mapTworld =
			Eigen::AlignedScaling2f(scaleToMap, scaleToMap) *
			Eigen::Translation2f((float)slam.maps[i].width  / 2.0f,
								 (float)slam.maps[i].height / 2.0f);
	}
}

static bool hs_pose_difference_larger_than(const Eigen::Vector3f& pose1,
										   const Eigen::Vector3f& pose2,
										   float distanceDiffThresh,
										   float angleDiffThresh) {
	if (((pose1.head<2>() - pose2.head<2>()).norm()) > distanceDiffThresh) {
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

constexpr float HECTOR_SLAM_UPDATE_FREE_FACTOR = 0.4f;
constexpr float HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR = 0.9f;

constexpr float HECTOR_SLAM_DISTANCE_THRESHOLD = 0.4f;
constexpr float HECTOR_SLAM_ANGLE_THRESHOLD = 0.9f;

constexpr int HECTOR_SLAM_ITERATIONS = 4;
constexpr int HECTOR_SLAM_ITERATIONS_FINAL = 6;

inline static void hs_mark_cell_free(HectorSlam &slam, HectorSlamOccGrid &map, unsigned int i) {
	if (map.updateIndex[i] < slam.updateIndex+0) {
		map.values[i] += log(HECTOR_SLAM_UPDATE_FREE_FACTOR);
	}
}

inline static void hs_mark_cell_occ(HectorSlam &slam, HectorSlamOccGrid &map, unsigned int i) {
	if (map.updateIndex[i] < slam.updateIndex+1) {
		if (map.values[i] < 50.0f) {
			map.values[i] += log(HECTOR_SLAM_UPDATE_OCCUPIED_FACTOR);
		}
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
											 const Eigen::Vector2i& begin,
											 const Eigen::Vector2i& end,
											 unsigned int max_length = UINT_MAX) {
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

    unsigned int startOffset = begin.y() * map.width + begin.x();

    //if x is dominant
    if(abs_dx >= abs_dy){
      int error_y = abs_dx / 2;
      hs_bresenham2D(slam, map, abs_dx, abs_dy, error_y, offset_dx, offset_dy, startOffset);
    }else{
      //otherwise y is dominant
      int error_x = abs_dy / 2;
      hs_bresenham2D(slam, map, abs_dy, abs_dx, error_x, offset_dy, offset_dx, startOffset);
    }

    unsigned int endOffset = end.y() * map.width + end.x();
    hs_mark_cell_occ(slam, map, endOffset);
}

void hs_update_map(HectorSlam &slam,
				   const Eigen::Vector3f &pose,
				   Eigen::Vector2f *points, size_t numPoints) {
	for (size_t map_i = 0; map_i < HECTOR_SLAM_MAP_RESOLUTIONS; map_i++) {
		HectorSlamOccGrid *map = &slam.maps[map_i];
		Eigen::Vector3f mapPose = hs_map_coords_pose(*map, pose);
		Eigen::Vector2f position = mapPose.head<2>();
		Eigen::Vector2i positioni = Eigen::Vector2i(position.x() + 0.5f,
													position.y() + 0.5f);

		Eigen::Affine2f poseTransform((Eigen::Translation2f(mapPose[0], mapPose[1]) *
									   Eigen::Rotation2Df(mapPose[2])));

		for (size_t i = 0; i < numPoints; i++) {
			Eigen::Vector2d point = poseTransform * points[i];
			point.x() += 0.5f;
			point.y() += 0.5f;

			Eigen::Vector2i pointi = point.cast<int>();

			if (pointi != positioni) {
				hs_update_line_bresenhami(slam, *map, positioni, pointi);
			}
		}
	}

	slam.updateIndex += 2;
}

void hs_update(HectorSlam &slam, Eigen::Vector2f *points, size_t numPoints) {
	Eigen::Vector3f estimate = slam.lastPosition;

	for (int i = HECTOR_SLAM_MAP_RESOLUTIONS - 1; i >= 0; i--) {
		estimate =
			hs_match_data(slam.maps[i], estimate,
						    (i == 0)
						  ? HECTOR_SLAM_ITERATIONS_FINAL
						  : HECTOR_SLAM_ITERATIONS,
						  points, numPoints);
	}

	if (hs_pose_difference_larger_than(estimate, slam.lastPosition,
									   HECTOR_SLAM_DISTANCE_THRESHOLD,
									   HECTOR_SLAM_ANGLE_THRESHOLD)) {
		hs_update_map(slam, estimate, points, numPoints);
	}

	slam.lastPosition = estimate;
}
