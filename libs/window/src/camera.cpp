#include "camera.h"

void update_camera(Camera &camera, const WindowFrameInfo &frame) {
	camera.yaw += (float)frame.mouse.dx * 0.005f;
	camera.yaw -= floor(camera.yaw);

	camera.pitch += (float)frame.mouse.dy * 0.005f;
	if (camera.pitch > 1.0f) {
		camera.pitch = 1.0f;
	} else if (camera.pitch < -1.0f) {
		camera.pitch = -1.0f;
	}

	vec3 move_delta = {};
	vec3 move_final = {};

	if (frame.keyboard.left)     {move_delta.x -= 1.0f;}
	if (frame.keyboard.right)    {move_delta.x += 1.0f;}

	if (frame.keyboard.up)       {move_delta.y += 1.0f;}
	if (frame.keyboard.down)     {move_delta.y -= 1.0f;}

	if (frame.keyboard.forward)  {move_delta.z -= 1.0f;}
	if (frame.keyboard.backward) {move_delta.z += 1.0f;}

	move_delta /= 0.5;

	if (move_delta.x != 0.0f) {
		move_final.x += cos(PI*2*camera.yaw)/move_delta.x;
		move_final.z += sin(PI*2*camera.yaw)/move_delta.x;
	}

	if (move_delta.z != 0.0f) {
		move_final.x += -sin(PI*2*camera.yaw)/move_delta.z;
		move_final.z += cos(PI*2*camera.yaw)/move_delta.z;
		move_final.y += sin((PI/2.0f)*camera.pitch)/move_delta.z;
	}

	move_final.y += move_delta.y;

	camera.position += move_final * (frame.keyboard.accelerate ? 0.5f : 0.1f);
}
