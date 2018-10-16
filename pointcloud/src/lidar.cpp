#include "lidar.h"
#include "opengl.h"
#include "utils.h"
#include "lidar_socket.h"
#include <sys/socket.h>
#include <stdio.h>
#include <semaphore.h>

void init_lidar(LidarContext &ctx) {
	GLuint vshader, fshader;

	vshader = create_shader_from_file("assets/shaders/polar.vsh", GL_VERTEX_SHADER);
	fshader = create_shader_from_file("assets/shaders/constant.fsh", GL_FRAGMENT_SHADER);

	ctx.shader.id = glCreateProgram();

	glAttachShader(ctx.shader.id, vshader);
	glAttachShader(ctx.shader.id, fshader);

	if (!link_shader_program(ctx.shader.id)) {
		panic("Could not compile the shader!");
	}

	ctx.shader.scale = glGetUniformLocation(ctx.shader.id, "scale");
	ctx.shader.color = glGetUniformLocation(ctx.shader.id, "color");

	glUseProgram(ctx.shader.id);
	glUniform1f(ctx.shader.scale, 7000.0f);
	glUniform3f(ctx.shader.color, 1.0f, 1.0f, 1.0f);

	glGenVertexArrays(1, &ctx.buffer_vao);
	glBindVertexArray(ctx.buffer_vao);

	glGenBuffers(1, &ctx.buffer_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ctx.buffer_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * scan_data_cap, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	init_lidar_socket(ctx.lidar_socket, "0.0.0.0", "6002");
}

void tick_lidar(LidarContext &ctx, const WindowFrameInfo &frame) {
	sem_wait(&ctx.lidar_socket.lock);

	size_t length = ctx.lidar_socket.scan_data_length[ctx.lidar_socket.scan_data_read_select];

	// printf("size %zu\n", length);
	glBindBuffer(GL_ARRAY_BUFFER, ctx.buffer_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * length, &ctx.lidar_socket.scan_data[ctx.lidar_socket.scan_data_read_select], GL_DYNAMIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0,
	// 				sizeof(vec2) * length,
	// 				&ctx.scan_data[ctx.scan_data_read_select]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	sem_post(&ctx.lidar_socket.lock);

	glUseProgram(ctx.shader.id);
	glBindVertexArray(ctx.buffer_vao);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, length);

	glUseProgram(0);
	glBindVertexArray(0);
}

void free_lidar(LidarContext &ctx) {
	glDeleteBuffers(1, &ctx.buffer_vbo);
	glDeleteVertexArrays(1, &ctx.buffer_vao);
}
