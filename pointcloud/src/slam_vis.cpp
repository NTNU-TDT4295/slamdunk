#include "slam_vis.h"
#include "linmath.h"
#include "opengl.h"
#include "utils.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/gtc/type_ptr.hpp>

static void slam_data_receiver(net_client_context *net_ctx) {
	SlamVisContext *ctx;
	ctx = (SlamVisContext *)net_ctx->user_data;

	assert(((ctx->width * ctx->height) % 4) == 0);

	while (!net_ctx->should_quit) {
		ssize_t err;
		uint8_t packet_type;

		err = recv(net_ctx->socket_fd, &packet_type, sizeof(uint8_t), 0);
		if (err < 0) {
			perror("recv");
			break;
		} else if (err == 0) {
			continue;
		}

		switch (packet_type) {
		case 0: {
			size_t bytes_read = 0;
			while (bytes_read < (ctx->width * ctx->height) / 4) {
				err = recv(net_ctx->socket_fd, ctx->read_buffer + bytes_read,
						   ((ctx->width*ctx->height)/4 * sizeof(uint8_t)) - bytes_read, 0);
				if (err < 0) {
					perror("recv");
					return;
				}

				bytes_read += err;
			}

			uint8_t *out_buffer = ctx->tex_buffer[ctx->tex_buffer_read ^ 0x1];
			uint8_t *read_buffer  = ctx->read_buffer;

			for (size_t i = 0; i < ctx->width * ctx->height; i++) {
				uint8_t val = (read_buffer[i / 4] >> (2 * (i % 4))) & 0x3;
				switch (val) {
				case 0x1: out_buffer[i] = 50;  break;
				case 0x2: out_buffer[i] = 255; break;
				default:  out_buffer[i] = 0;   break;
				}
			}

			sem_wait(&ctx->lock);
			ctx->tex_buffer_read ^= 0x1;
			sem_post(&ctx->lock);
		} break;

		case 1: {
			uint32_t buffer[3];
			err = recv(net_ctx->socket_fd, buffer, sizeof(buffer), 0);
			if (err < 0) {
				perror("recv");
				return;
			}

			vec3 pos = vec3((float)buffer[0] / 1000.0f,
							(float)buffer[1] / 1000.0f,
							(float)buffer[2] / 1000000.0f);

			printf("%f %f - %f\n", pos.x, pos.y, pos.z);
		} break;

		default:
			print_error("net", "Got invalid packet type %i!\n", packet_type);
			break;
		}
	}
}

void init_slam_vis(SlamVisContext &ctx) {
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

	ctx.width = 1024;
	ctx.height = 1024;

	int mapWidth = ctx.width;
	int mapHeight = ctx.height;

	ctx.tex_buffer[0] = (uint8_t *)calloc(mapWidth * mapHeight, sizeof(uint8_t));
	ctx.tex_buffer[1] = (uint8_t *)calloc(mapWidth * mapHeight, sizeof(uint8_t));

	ctx.read_buffer = (uint8_t *)calloc((mapWidth * mapHeight) / 4, sizeof(uint8_t));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mapWidth, mapHeight, 0,
				 GL_RED, GL_UNSIGNED_BYTE, ctx.tex_buffer[0]);

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	sem_init(&ctx.lock, 0, 1);
	ctx.net.client_callback = slam_data_receiver;
	ctx.net.user_data = &ctx;
	net_init(&ctx.net, "0.0.0.0", "6000");
}

void tick_slam_vis(SlamVisContext &ctx, const WindowFrameInfo &info) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ctx.shader.id);
	glBindVertexArray(ctx.quad_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx.texture);

	sem_wait(&ctx.lock);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ctx.width, ctx.height, 0,
				 GL_RED, GL_UNSIGNED_BYTE, ctx.tex_buffer[ctx.tex_buffer_read]);
	sem_post(&ctx.lock);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void free_slam_vis(SlamVisContext &ctx) {
	net_shutdown(&ctx.net);
	sem_destroy(&ctx.lock);
	free(ctx.tex_buffer[0]);
	free(ctx.tex_buffer[1]);
	free(ctx.read_buffer);
}
