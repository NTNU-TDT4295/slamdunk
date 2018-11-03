#include "point_cloud.h"

#include "opengl.h"
#include "utils.h"
#include "octree_loader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <sys/socket.h>

void point_cloud_net_client_callback(net_client_context *net_ctx) {
	PointCloudContext *ctx;
	ctx = (PointCloudContext *)net_ctx->user_data;
	(void) ctx;

	while (!net_ctx->should_quit) {
		int32_t buffer[3];
		size_t bytes_read = 0;

		while (bytes_read < sizeof(buffer)) {
			int err = recv(net_ctx->socket_fd, ((uint8_t *)buffer) + bytes_read, sizeof(buffer) - bytes_read, 0);
			if (err < 0) {
				perror("recv");
				return;
			} else if (err == 0) {
				return;
			}

			bytes_read += err;
		}

		vec3 point;
		point.x = (float)buffer[0] / 1000.0f;
		point.y = (float)buffer[1] / 1000.0f;
		point.z = (float)buffer[2] / 1000.0f;

		// printf("%f %f %f\n", point.x, point.y, point.z);

		ctx->queue.push(point);
	}
}

constexpr float axis_size = 1000.0f;
constexpr OctreePoint axis_lines[] = {
	{ -axis_size, 0.0f, 0.0f }, {  axis_size, 0.0f, 0.0f },
	{ 0.0f, -axis_size, 0.0f }, {  0.0f, axis_size, 0.0f },
	{ 0.0f, 0.0f, -axis_size }, {  0.0f, 0.0f, axis_size },
};
constexpr size_t axis_lines_length = sizeof(axis_lines) / sizeof(OctreePoint);

static void init_axis(PointCloudContext &ctx) {
	GLuint vao, buffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_lines), axis_lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreePoint), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ctx.axis_vao = vao;
	ctx.axis_vbo = buffer;
}

void init_point_cloud(PointCloudContext &ctx) {
	ctx.camera.position = { 0.0f, 1.8f, 0.0f };

	GLuint vshader, fshader;
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

	octree_render_init(ctx.octree_render);
	ctx.octree_render.matrix_uniform = ctx.shader.in_matrix;


	Octree *octree = &ctx.octree_render.octree;
	octree->box.center = { 0.0f, 0.0f, 0.0f };
	octree->box.radius = 10000.0f;

	// octree_load_obj(octree, "assets/models/mountain.obj");

	point_queue_init(ctx.queue, 1024*1024);

	init_axis(ctx);

	ctx.net.client_callback = point_cloud_net_client_callback;
	ctx.net.user_data = &ctx;
	net_init(&ctx.net, "0.0.0.0", "6000");
}

void tick_point_cloud(PointCloudContext &ctx, const WindowFrameInfo &frame) {
	update_camera(ctx.camera, frame);

	vec3 point;
	while (ctx.queue.pop(&point)) {
		OctreePoint p = { point.x, point.y, point.z };
		ctx.octree_render.octree.insert(p);
	}

	if (frame.keyboard.toggle_boundary && !ctx.display_octree_boundaries_down) {
		ctx.display_octree_boundaries = !ctx.display_octree_boundaries;
	}
	ctx.display_octree_boundaries_down = frame.keyboard.toggle_boundary;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ctx.shader.id);

	mat4 matrix;

	float aspect = (float)frame.window.width/(float)frame.window.height;
	matrix = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);
	matrix = glm::rotate(matrix, PI/2 * ctx.camera.pitch, vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, PI*2 * ctx.camera.yaw,   vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::translate(matrix, -ctx.camera.position);

	glUniformMatrix4fv(ctx.shader.in_projection_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	if (ctx.display_octree_boundaries) {
		glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
		glUniform3f(ctx.shader.emission_color, 0.0f, 1.0f, 1.0f);
		octree_render_bounds(ctx.octree_render);
	}

	glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 1.0f, 1.0f, 1.0f);
	octree_render(ctx.octree_render);

	matrix = mat4(1.0f);
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	glBindVertexArray(ctx.axis_vao);
	glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, axis_lines_length);
	glBindVertexArray(0);

	glUseProgram(0);
}

void free_point_cloud(PointCloudContext &ctx) {
	net_shutdown(&ctx.net);
	glDeleteVertexArrays(1, &ctx.axis_vao);
	glDeleteBuffers(1, &ctx.axis_vbo);
}
