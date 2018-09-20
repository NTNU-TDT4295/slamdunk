#pragma once

#include "window.h"
#include "linmath.h"
#include "model_loader_obj.h"
#include "camera.h"

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

	model mdl;
	model sphere;
};

void init_simulator(SimulatorContext &);
void tick_simulator(SimulatorContext &, const WindowFrameInfo &);
