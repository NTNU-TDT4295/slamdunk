#pragma once

#include "window.h"
#include "linmath.h"

struct Camera {
	vec3 position;
	float yaw, pitch;
};

void update_camera(Camera &, const WindowFrameInfo &);
