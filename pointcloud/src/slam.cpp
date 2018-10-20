#include "slam.h"
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include "opengl.h"
#include "utils.h"

#include <glm/gtc/type_ptr.hpp>

// static int send_point_data(int fd, vec3 point) {
// 	int32_t buffer[3];

// 	if (fd == -1) {
// 		return -1;
// 	}

// 	buffer[0] = (int32_t)(point.x * 1000.0f);
// 	buffer[1] = (int32_t)(point.y * 1000.0f);
// 	buffer[2] = (int32_t)(point.z * 1000.0f);

// 	ssize_t err;
// 	err = send(fd, buffer, sizeof(buffer), 0);
// 	if (err < 0) {
// 		perror("send");
// 		return -1;
// 	}

// 	return 0;
// }

void init_slam(SlamContext &ctx) {
	ctx.numPoints = 0;
	ctx.capPoints = scan_data_cap;
	ctx.points = (vec2 *)calloc(ctx.capPoints, sizeof(vec2));

	hs_init(ctx.slam);

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

	int mapWidth = ctx.slam.width;
	int mapHeight = ctx.slam.height;

	ctx.tex_buffer = (uint8_t *)calloc(mapWidth * mapHeight, sizeof(uint8_t));

	memset(ctx.tex_buffer, 0, mapHeight * mapHeight * sizeof(uint8_t));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mapWidth, mapHeight, 0,
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void tick_slam(SlamContext &ctx, const WindowFrameInfo &info) {
	sem_wait(&ctx.lidar_socket.lock);

	size_t length = ctx.lidar_socket.scan_data_length[ctx.lidar_socket.scan_data_read_select];
	// printf("%zu\n", length);

	float scale = 1.0f / ctx.slam.maps[0].cellSize;

	ctx.numPoints = 0;

	for (size_t i = 0; i < length; i++) {
		vec2 p = ctx.lidar_socket.scan_data[ctx.lidar_socket.scan_data_read_select][i];
		float angle = p.x * M_PI / 180.0f;
		float dist  = (p.y / 1000.0f) * scale;

		if (dist > 0.1f) {
			if (ctx.numPoints >= ctx.capPoints) {
				printf("Too many points in tick!\n");
				break;
			}

			vec2 point = vec2(cos(angle) * dist, sin(angle) * dist);

			//printf("setting point %f %f\n", point.x, point.y);
			ctx.points[ctx.numPoints] = point;

			ctx.numPoints += 1;
		}
	}

	sem_post(&ctx.lidar_socket.lock);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ctx.numPoints > 0) {
		printf("Updating, %zu\n", ctx.numPoints);
		hs_update(ctx.slam,
				  ctx.points,
				  ctx.numPoints);

		printf("position: %f %f - %f\n",
			   ctx.slam.lastPosition.x,
			   ctx.slam.lastPosition.y,
			   ctx.slam.lastPosition.z);
	}

	if (info.keyboard.forward && !ctx.btn_down) {
		ctx.res = (ctx.res + 1) % HECTOR_SLAM_MAP_RESOLUTIONS;
	}
	ctx.btn_down = info.keyboard.forward;

	HectorSlamOccGrid &map = ctx.slam.maps[ctx.res];

	memset(ctx.tex_buffer, 0, map.width * map.height);

	for (size_t i = 0; i < map.width * map.height; i++) {
		if (hs_is_occupied(map.values[i])) {
			ctx.tex_buffer[i] = 255;
		} else if (hs_is_free(map.values[i])) {
			ctx.tex_buffer[i] = 50;
		}
	}

	glUseProgram(ctx.shader.id);
	glBindVertexArray(ctx.quad_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx.texture);

	// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, map.width, map.height,
	// 				GL_RED, GL_UNSIGNED_BYTE, ctx.tex_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, map.width, map.height, 0,
				 GL_RED, GL_UNSIGNED_BYTE, ctx.tex_buffer);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void free_slam(SlamContext &ctx) {
	free_lidar_socket(ctx.lidar_socket);
	hs_free(ctx.slam);

	free(ctx.points);
	free(ctx.tex_buffer);
}
