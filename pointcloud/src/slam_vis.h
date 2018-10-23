#pragma once

#include "window.h"
#include <stdint.h>
#include <stddef.h>
#include "net.h"
#include <semaphore.h>

struct SlamVisContext {
	unsigned int quad_vao;
	unsigned int texture;

	struct {
		unsigned int id;
		unsigned int in_matrix;
		unsigned int in_projection_matrix;
		unsigned int in_tex;
	} shader;

	uint8_t *read_buffer;

	sem_t lock;
	uint8_t *tex_buffer[2];
	int tex_buffer_read;
	size_t width, height;

	net_context net;
};

void init_slam_vis(SlamVisContext &ctx);
void tick_slam_vis(SlamVisContext &ctx, const WindowFrameInfo &info);
void free_slam_vis(SlamVisContext &ctx);
