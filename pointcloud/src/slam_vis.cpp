#include "slam_vis.h"
#include "linmath.h"
#include "opengl.h"
#include "utils.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include "slam_vis_net.h"

#include <glm/gtc/type_ptr.hpp>

static int recv_all(int fd, void *data, size_t num_bytes) {
	size_t bytes_recv = 0;
	ssize_t err;

	while (bytes_recv < num_bytes) {
		err = recv(fd, (uint8_t *)data + bytes_recv, num_bytes - bytes_recv, 0);
		if (err <= 0) {
			if (err < 0) {
				perror("recv");
			}
			return -1;
		}
		bytes_recv += err;
	}

	return 0;
}

static void slam_data_receiver(net_client_context *net_ctx) {
	SlamVisContext *ctx;
	ctx = (SlamVisContext *)net_ctx->user_data;

	assert(((ctx->width * ctx->height) % 4) == 0);

	while (!net_ctx->should_quit) {
		int err;
		uint8_t packet_type;

		err = recv_all(net_ctx->socket_fd, &packet_type, sizeof(uint8_t));
		if (err) {
			return;
		}

		switch (packet_type) {
		case SLAM_PACKET_MAP: {
			size_t bytes_total = (SLAM_MAP_WIDTH * SLAM_MAP_HEIGHT) / 4;
			err = recv_all(net_ctx->socket_fd, ctx->read_buffer, bytes_total);
			if (err) {
				return;
			}

			uint8_t *out_buffer  = ctx->tex_buffer[ctx->tex_buffer_read ^ 0x1];
			uint8_t *read_buffer = ctx->read_buffer;

			for (size_t i = 0; i < SLAM_MAP_WIDTH * SLAM_MAP_HEIGHT; i++) {
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

		case SLAM_PACKET_TILE_BEGIN: {
			uint8_t *out_buffer  = ctx->tex_buffer[ctx->tex_buffer_read ^ 0x1];
			uint8_t *read_buffer = ctx->tex_buffer[ctx->tex_buffer_read];

			memcpy(out_buffer, read_buffer, SLAM_MAP_WIDTH * SLAM_MAP_HEIGHT);
		} break;

		case SLAM_PACKET_TILE_DONE: {
			sem_wait(&ctx->lock);
			ctx->tex_buffer_read ^= 0x1;
			sem_post(&ctx->lock);
		} break;

		case SLAM_PACKET_TILE: {
			uint8_t header[2];
			err = recv_all(net_ctx->socket_fd, header, sizeof(header));
			if (err) {
				return;
			}

			uint8_t chunk_x = header[0];
			uint8_t chunk_y = header[1];

			uint8_t buffer[(SLAM_MAP_TILE_SIZE*SLAM_MAP_TILE_SIZE) / 4] = {0};

			err = recv_all(net_ctx->socket_fd, buffer, sizeof(buffer));
			if (err) {
				return;
			}

			size_t map_row_stride = SLAM_MAP_WIDTH - SLAM_MAP_TILE_SIZE;
			size_t map_offset =
				((size_t)chunk_x * SLAM_MAP_TILE_SIZE) +
				((size_t)chunk_y * SLAM_MAP_TILE_SIZE) * SLAM_MAP_WIDTH;

			uint8_t *out_buffer = ctx->tex_buffer[ctx->tex_buffer_read ^ 0x1];

			for (size_t i = 0; i < SLAM_MAP_TILE_SIZE*SLAM_MAP_TILE_SIZE; i++) {
				size_t j = map_offset + i + (i / SLAM_MAP_TILE_SIZE) * map_row_stride;

				uint8_t val = (buffer[i / 4] >> ((i % 4) * 2)) & 0x3;

				switch (val) {
				case 0x1: out_buffer[j] = 50;  break;
				case 0x2: out_buffer[j] = 255; break;
				default:  out_buffer[j] = 0;   break;
				}
			}

		} break;

		case SLAM_PACKET_POSE: {
			int32_t buffer[3];
			err = recv_all(net_ctx->socket_fd, buffer, sizeof(buffer));
			if (err) {
				return;
			}

			vec3 pos = vec3((float)buffer[0] / 1000.0f,
							(float)buffer[1] / 1000.0f,
							(float)buffer[2] / 1000000.0f);

			ctx->pose = pos;

			if (ctx->path_last_page->num_entries >= SLAM_PATH_PAGE_SIZE) {
				ctx->path_last_page->next = (SlamPathPage *)calloc(1, sizeof(SlamPathPage));
				ctx->path_last_page = ctx->path_last_page->next;
			}
			ctx->path_last_page->entries[ctx->path_last_page->num_entries] = pos;
			ctx->path_last_page->num_entries += 1;
		} break;

		case SLAM_PACKET_RESET_PATH: {
			ctx->should_reset_path = true;
		} break;

		default:
			print_error("net", "Got invalid packet type %i!\n", packet_type);
			break;
		}
	}
}


constexpr float pose_marker_data[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,

	 0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f,

	// 0.0f, 0.0f, 1.0f,
	// 0.0f, 1.0f, 1.0f,

	// 0.0f, 1.0f, 1.0f,
	// 0.3f, 0.7f, 1.0f,

	// 0.0f, 1.0f, 1.0f,
	// -0.3f, 0.7f, 1.0f,
};
constexpr size_t pose_marker_points = sizeof(pose_marker_data) / (sizeof(float) * 3);

static void init_slam_path_page(SlamPathPage *page) {
	glGenVertexArrays(1, &page->vao);
	glBindVertexArray(page->vao);

	glGenBuffers(1, &page->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, page->vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(page->entries), page->entries, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	page->inited = true;
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

	ctx.width = SLAM_MAP_WIDTH;
	ctx.height = SLAM_MAP_HEIGHT;

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

	ctx.tex_shader.id = glCreateProgram();

	glAttachShader(ctx.tex_shader.id, vshader);
	glAttachShader(ctx.tex_shader.id, fshader);

	if (!link_shader_program(ctx.tex_shader.id)) {
		panic("Could not compile the shader!");
	}

	ctx.tex_shader.in_matrix            = glGetUniformLocation(ctx.tex_shader.id, "in_matrix");
	ctx.tex_shader.in_projection_matrix = glGetUniformLocation(ctx.tex_shader.id, "in_projection_matrix");
	ctx.tex_shader.in_tex = glGetUniformLocation(ctx.tex_shader.id, "tex");

	glUseProgram(ctx.tex_shader.id);

	mat4 mat (1.0f);

	glUniformMatrix4fv(ctx.tex_shader.in_projection_matrix, 1, GL_FALSE, glm::value_ptr(mat));
	glUniformMatrix4fv(ctx.tex_shader.in_matrix, 1, GL_FALSE, glm::value_ptr(mat));
	glUniform1i(ctx.tex_shader.in_tex, 0);

	glUseProgram(0);



	vshader = create_shader_from_file("assets/shaders/test.vsh", GL_VERTEX_SHADER);
	fshader = create_shader_from_file("assets/shaders/test.fsh", GL_FRAGMENT_SHADER);

	ctx.shader.id = glCreateProgram();

	glAttachShader(ctx.shader.id, vshader);
	glAttachShader(ctx.shader.id, fshader);

	if (!link_shader_program(ctx.shader.id)) {
		panic("Could not compile the shader!");
	}

	ctx.shader.in_matrix            = glGetUniformLocation(ctx.shader.id, "in_matrix");
	ctx.shader.in_projection_matrix = glGetUniformLocation(ctx.shader.id, "in_projection_matrix");
	ctx.shader.emission_color       = glGetUniformLocation(ctx.shader.id, "emission_color");
	ctx.shader.diffuse_color        = glGetUniformLocation(ctx.shader.id, "diffuse_color");

	glUseProgram(ctx.shader.id);
	glUniformMatrix4fv(ctx.shader.in_projection_matrix, 1, GL_FALSE, glm::value_ptr(mat));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(mat));


	glGenVertexArrays(1, &ctx.pose_marker_vao);
	glBindVertexArray(ctx.pose_marker_vao);

	glGenBuffers(1, &ctx.pose_marker_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ctx.pose_marker_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(pose_marker_data),
				 pose_marker_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ctx.path = ctx.path_last_page = (SlamPathPage *)calloc(1, sizeof(SlamPathPage));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	sem_init(&ctx.lock, 0, 1);
	ctx.net.client_callback = slam_data_receiver;
	ctx.net.user_data = &ctx;
	net_init(&ctx.net, "0.0.0.0", "6000");
}

void tick_slam_vis(SlamVisContext &ctx, const WindowFrameInfo &info) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(ctx.tex_shader.id);
	glBindVertexArray(ctx.quad_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ctx.texture);

	sem_wait(&ctx.lock);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ctx.width, ctx.height, 0,
				 GL_RED, GL_UNSIGNED_BYTE, ctx.tex_buffer[ctx.tex_buffer_read]);
	sem_post(&ctx.lock);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Draw pose marker
	glUseProgram(ctx.shader.id);

	glBindVertexArray(ctx.pose_marker_vao);
	glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 1.0f, 0.0f, 0.0f);
	mat4 matrix;
	matrix = mat4(1.0f);
	float scale = 1.0f / (SLAM_MAP_METERS_PER_PIXEL * (float)SLAM_MAP_WIDTH / 2.0f);
	matrix = glm::translate(matrix, vec3(ctx.pose.x, ctx.pose.y, 0.0f) * scale);
	matrix = glm::rotate(matrix, ctx.pose.z, vec3(0.0f, 0.0f, 1.0f));
	matrix = glm::scale(matrix, vec3(scale, scale, 0.0f));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, pose_marker_points);

	glUniform3f(ctx.shader.emission_color, 0.0f, 1.0f, 0.0f);
	glLineWidth(1.0f);
	matrix = mat4(1.0f);
	matrix = glm::scale(matrix, vec3(scale, scale, 0.0f));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	if (ctx.should_reset_path) {
		for (SlamPathPage *page = ctx.path->next; page != NULL;) {
			glDeleteVertexArrays(1, &page->vao);
			glDeleteBuffers(1, &page->vbo);

			SlamPathPage *last_page = page;
			page = page->next;
			free(last_page);
		}

		ctx.path->num_entries = 0;
		ctx.path->last_vbo_update = -1;
		ctx.path_last_page = ctx.path;

		ctx.should_reset_path = false;
	}

	for (SlamPathPage *page = ctx.path; page != NULL; page = page->next) {
		size_t num_entries = page->num_entries;
		if (num_entries > 0) {
			if ((ssize_t)num_entries > page->last_vbo_update) {
				if (!page->inited) {
					init_slam_path_page(page);
				}

				glBindBuffer(GL_ARRAY_BUFFER, page->vbo);
				// glBufferSubData(GL_ARRAY_BUFFER,
				// 				page->last_vbo_update * sizeof(vec3),
				// 				(num_entries - page->last_vbo_update) * sizeof(vec3),
				// 				page->entries);
				glBufferData(GL_ARRAY_BUFFER, sizeof(page->entries),
							 page->entries, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				page->last_vbo_update = num_entries;
			}

			glBindVertexArray(page->vao);
			glDrawArrays(GL_LINE_STRIP, 0, num_entries);
		}
	}
}

void free_slam_vis(SlamVisContext &ctx) {
	net_shutdown(&ctx.net);
	sem_destroy(&ctx.lock);
	free(ctx.tex_buffer[0]);
	free(ctx.tex_buffer[1]);
	free(ctx.read_buffer);
}
