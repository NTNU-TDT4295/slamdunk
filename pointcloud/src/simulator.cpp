#include "simulator.h"

#include "opengl.h"
#include "utils.h"
#include "octree.h"
#include <stdio.h>
#include <sys/socket.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float max_ray_length = 8.0f;

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

static void simulator_in_client(net_client_context *net_ctx) {
	SimulatorContext *ctx;
	ctx = (SimulatorContext *)net_ctx->user_data;

	while (!net_ctx->should_quit) {
		int16_t buffer[4];

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

		int16_t w, x, y, z;

		w = buffer[0];
		x = buffer[1];
		y = buffer[2];
		z = buffer[3];

		float scale = (1.0f / (1 << 14));
		ctx->sensor.rotation = quat(w * scale, x * scale, z * scale, y * scale);

		printf("%i %i %i %i\n", w, x, y, z);
		printf("%f %f %f %f\n",
			   ctx->sensor.rotation.w,
			   ctx->sensor.rotation.x,
			   ctx->sensor.rotation.y,
			   ctx->sensor.rotation.z);
	}
}

void init_simulator(SimulatorContext &ctx) {
	ctx.camera.position = { 0.0f, 1.8f,  0.0f };
	ctx.sensor.position = { 0.0f, 1.8f, -1.0f };

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

	arena transient;

	arena_init(&transient, MEGABYTE(1));
	if (!load_obj_file(transient, STR("assets/models/room.obj"), &ctx.mdl)) {
		panic("Failed to load model room!");
	}
	if (!load_obj_file(transient, STR("assets/models/head.obj"), &ctx.sphere)) {
		panic("Failed to load model spehere!");
	}
	arena_free(&transient);

	ctx.mdl_mesh = new TriangleMesh("assets/models/room.obj");

	constexpr OctreePoint line[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
	};

	GLuint vao, buffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(line),
				 &line, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreePoint), 0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ctx.line_vao = vao;

	ctx.socket_fd = net_client_connect("127.0.0.1", "6002");

	ctx.net_in.user_data = &ctx;
	ctx.net_in.client_callback = simulator_in_client;
	net_init(&ctx.net_in, "0.0.0.0", "6001");

	// ctx.sensor.rotation = glm::angleAxis(PI / 4, vec3(1.0f, 0.0f, 0.0f));
}

void tick_simulator(SimulatorContext &ctx, const WindowFrameInfo &frame) {
	update_camera(ctx.camera, frame);

	vec3 move_delta = {};

	if (frame.keyboard.device_left)     {move_delta.x -= 1.0f;}
	if (frame.keyboard.device_right)    {move_delta.x += 1.0f;}

	if (frame.keyboard.device_up)       {move_delta.y += 1.0f;}
	if (frame.keyboard.device_down)     {move_delta.y -= 1.0f;}

	if (frame.keyboard.device_forward)  {move_delta.z -= 1.0f;}
	if (frame.keyboard.device_backward) {move_delta.z += 1.0f;}

	move_delta /= 32.0f;

	ctx.sensor.position += move_delta;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ctx.shader.id);

	// Calculate camera matrix
	mat4 matrix;

	float aspect = (float)frame.window.width/(float)frame.window.height;
	matrix = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);
	matrix = glm::rotate(matrix, PI/2 * ctx.camera.pitch, vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, PI*2 * ctx.camera.yaw,   vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::translate(matrix, -ctx.camera.position);

	glUniformMatrix4fv(ctx.shader.in_projection_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	// Draw the world
	matrix = mat4(1.0f);
	matrix = glm::translate(matrix, vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	glBindVertexArray(ctx.mdl.vao);
	glUniform3f(ctx.shader.diffuse_color, 1.0f, 1.0f, 1.0f);
	glUniform3f(ctx.shader.emission_color, 0.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, ctx.mdl.num_vertex);
	glBindVertexArray(0);

	mat4 rotation_matrix = glm::mat4_cast(ctx.sensor.rotation);

	// Draw sensor sphere
	matrix = mat4(1.0f);
	matrix = glm::translate(matrix, ctx.sensor.position);
	matrix = matrix * rotation_matrix;
	// matrix = glm::rotate(matrix, -PI / 2.0f, vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::scale(matrix, vec3(0.1f));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	glBindVertexArray(ctx.sphere.vao);
	glUniform3f(ctx.shader.diffuse_color, 1.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 0.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, ctx.sphere.num_vertex);
	glBindVertexArray(0);

	// Draw sensor rays
	glBindVertexArray(ctx.line_vao);
	glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 1.0f, 0.0f, 0.0f);

	constexpr size_t num_lines = 360;

	if (num_lines > ctx.frame_point_cloud_data_len) {
		float *new_data;
		new_data = (float *)realloc(ctx.frame_point_cloud_data,
									sizeof(float) * 3 * num_lines);

		if (new_data) {
			ctx.frame_point_cloud_data = new_data;
			ctx.frame_point_cloud_data_len = num_lines;
		} else {
			printf("Failed to allocate memory.\n");
		}
	}

	constexpr size_t scan_data_point_size = 5;
	uint8_t scan_data[num_lines * scan_data_point_size];

	for (size_t i = 0; i < num_lines; i++) {

		float angle = ((float)i / (float)num_lines) * 2 * PI;
		float ray_length;

		vec3 point = vec3(cos(angle), 0.0f, sin(angle));
		vec3 direction = rotation_matrix * vec4(point, 1.0f);

		Ray ray = Ray(ctx.sensor.position, direction);

		ray_length = ctx.mdl_mesh->GetIntersection(ray);

		uint16_t angle_q = (uint16_t)((angle * 180.0f / M_PI) * 64.0f) << 1;

		scan_data[i*scan_data_point_size + 0] = 0xa5;
		scan_data[i*scan_data_point_size + 1] = angle_q & 0xff;
		scan_data[i*scan_data_point_size + 2] = (angle_q >> 8);

		if (ray_length > 0 && ray_length <= max_ray_length) {
			uint16_t dist_q = (uint16_t)(ray_length * 4.0f * 1000.0f);
			scan_data[i*scan_data_point_size + 3] = dist_q & 0xff;
			scan_data[i*scan_data_point_size + 4] = (dist_q >> 8);
		} else {
			scan_data[i*scan_data_point_size + 3] = 0;
			scan_data[i*scan_data_point_size + 4] = 0;
		}


		if (ray_length < 0 || ray_length > max_ray_length) {
			continue;
		}

		point = point * ray_length;

		if (i < ctx.frame_point_cloud_data_len) {
			ctx.frame_point_cloud_data[(i*3) + 0] = point.x;
			ctx.frame_point_cloud_data[(i*3) + 1] = point.y;
			ctx.frame_point_cloud_data[(i*3) + 2] = point.z;
		}

		// Draw line
		matrix = mat4(1.0f);
		matrix = glm::translate(matrix, ctx.sensor.position);
		matrix = matrix * rotation_matrix;
		matrix = glm::scale(matrix, point);
		glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));
		glDrawArrays(GL_LINES, 0, 2);

		// if (frame.tick % 10 == 0 || true) {
		// 	point = (rotation_matrix * vec4(point, 1.0f));
		// 	point += ctx.sensor.position;
		// 	int err = send_point_data(ctx.socket_fd, point);
		// 	if (err) {
		// 		ctx.socket_fd = -1;
		// 	}
		// }
	}

	if (ctx.socket_fd > 0 && (frame.tick % 10) == 0) {
		size_t bytes_sent = 0;
		while (bytes_sent < sizeof(scan_data)) {
			ssize_t err;
			err = send(ctx.socket_fd, scan_data, sizeof(scan_data), 0);
			if (err < 0) {
				ctx.socket_fd = -1;
				perror("send");
				break;
			}
			bytes_sent += err;
		}
	}


	glBindVertexArray(0);
}
