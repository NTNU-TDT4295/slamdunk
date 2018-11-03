#pragma once

#include "window.h"
#include "linmath.h"
#include "model_loader_obj.h"
#include "camera.h"
#include "net.h"
#include "TriangleMesh.h"

struct SimulatorContext {
	Camera camera;

	struct {
		unsigned int id;
		unsigned int in_matrix;
		unsigned int in_projection_matrix;
		unsigned int emission_color;
		unsigned int diffuse_color;
	} shader;

	struct {
		vec3 position;
		quat rotation;
	} sensor;

	unsigned int line_vao;

	int socket_fd;

	net_context net_in;

	model mdl;
	model sphere;

	TriangleMesh *mdl_mesh;

	float *frame_point_cloud_data;
	size_t frame_point_cloud_data_len;
	unsigned int frame_point_cloud_vao;
	unsigned int frame_point_cloud_vbo;
};

void init_simulator(SimulatorContext &);
void tick_simulator(SimulatorContext &, const WindowFrameInfo &);
