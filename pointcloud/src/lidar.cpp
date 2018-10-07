#include "lidar.h"
#include "opengl.h"
#include "utils.h"
#include <sys/socket.h>
#include <stdio.h>

static void lidar_in_client(net_client_context *net_ctx) {
	LidarContext *ctx;
	ctx = (LidarContext *)net_ctx->user_data;

	uint8_t buffer[5];
	size_t bytes_read = 0;
	size_t num_aligns = 0;
	float last_angle = 0;

	while (!net_ctx->should_quit) {
		while (bytes_read < sizeof(buffer)) {
			int err = recv(net_ctx->socket_fd,
						   ((uint8_t *)buffer) + bytes_read,
						   sizeof(buffer) - bytes_read, 0);
			if (err < 0) {
				perror("recv");
				return;
			} else if (err == 0) {
				return;
			}

			bytes_read += err;
		}

		// printf("%2x %2x %2x %2x %2x %2x ",
		// 	   buffer[0], buffer[1], buffer[2],
		// 	   buffer[3], buffer[4], buffer[5]);

		if (buffer[0] != 0xa5) {
			printf("Missalignment...");
			bytes_read = 0;
			for (unsigned int i = 1; i < sizeof(buffer); i++) {
				if (buffer[i] == 0xa5) {
					printf(" offsetting %i", i);
					bytes_read = sizeof(buffer) - i;
					memmove(&buffer[0], &buffer[i], sizeof(buffer) - i);
				}
			}
			printf("\n");
			num_aligns += 1;
			continue;
		}

		uint16_t angle_q;
		float angle;
		float dist;

		angle_q = ((buffer[2] << 8) | (buffer[1]));
		angle = (float) (angle_q >> 1);
		angle = angle / 64.0;

		dist = (float)((buffer[4] << 8) | buffer[3]) / 4.0f;

		// printf("%zu %10f %10f\n", num_aligns, angle, dist);
		bytes_read = 0;

		// printf("%f %f\n", angle, dist);

		if (angle < last_angle - 50.0f) {
			sem_wait(&ctx->lock);
			ctx->scan_data_read_select ^= 1;
			ctx->scan_data_length[ctx->scan_data_read_select ^ 1] = 0;
			sem_post(&ctx->lock);
		}
		last_angle = angle;

		int write_select = ctx->scan_data_read_select ^ 1;

		if (ctx->scan_data_length[write_select] >= scan_data_cap) {
			printf("Passed scan data cap.\n");
			continue;
		}

		ctx->scan_data[write_select][ctx->scan_data_length[write_select]] = vec2(angle, dist);
		ctx->scan_data_length[write_select] += 1;
	}
}

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

	sem_init(&ctx.lock, 0, 1);

	ctx.net_in.user_data = &ctx;
	ctx.net_in.client_callback = lidar_in_client;
	net_init(&ctx.net_in, "0.0.0.0", "6002");
}

void tick_lidar(LidarContext &ctx, const WindowFrameInfo &frame) {
	sem_wait(&ctx.lock);

	size_t length = ctx.scan_data_length[ctx.scan_data_read_select];

	// printf("size %zu\n", length);
	glBindBuffer(GL_ARRAY_BUFFER, ctx.buffer_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * length, &ctx.scan_data[ctx.scan_data_read_select], GL_DYNAMIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0,
	// 				sizeof(vec2) * length,
	// 				&ctx.scan_data[ctx.scan_data_read_select]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	sem_post(&ctx.lock);

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
