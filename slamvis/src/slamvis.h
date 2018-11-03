#pragma once

#include "window.h"
#include <stdint.h>
#include <stddef.h>
#include "net.h"
#include "linmath.h"
#include <semaphore.h>

constexpr size_t SLAM_PATH_PAGE_SIZE = 1024;
struct SlamPathPage {
	vec3 entries[SLAM_PATH_PAGE_SIZE];
	size_t num_entries;

	// num_entries the last time the opengl buffer was updated.
	ssize_t last_vbo_update;
	unsigned int vao, vbo;
	SlamPathPage *next;
	bool inited;
};

struct SlamVisContext {
	unsigned int quad_vao;
	unsigned int texture;

	struct {
		unsigned int id;
		unsigned int in_matrix;
		unsigned int in_projection_matrix;
		unsigned int in_tex;
	} tex_shader;

	struct {
		unsigned int id;
		unsigned int in_matrix;
		unsigned int in_projection_matrix;
		unsigned int emission_color;
		unsigned int diffuse_color;
	} shader;

	unsigned int pose_marker_vbo;
	unsigned int pose_marker_vao;

	uint8_t *read_buffer;

	sem_t lock;
	uint8_t *tex_buffer[2];
	int tex_buffer_read;
	size_t width, height;

	vec3 pose;

	net_context net;

	bool should_reset_path;
	SlamPathPage *path;
	SlamPathPage *path_last_page;
};

void init_slam_vis(SlamVisContext &ctx);
void tick_slam_vis(SlamVisContext &ctx, const WindowFrameInfo &info);
void free_slam_vis(SlamVisContext &ctx);
