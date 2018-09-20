#include "simulator.h"

#include "opengl.h"
#include "utils.h"
#include "octree.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void init_simulator(SimulatorContext &ctx) {
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

	arena transient;

	arena_init(&transient, MEGABYTE(1));
	if (!load_obj_file(transient, STR("assets/models/room.obj"), &ctx.mdl)) {
		panic("Failed to load model room!");
	}
	if (!load_obj_file(transient, STR("assets/models/sphere.obj"), &ctx.sphere)) {
		panic("Failed to load model spehere!");
	}
	arena_free(&transient);

	constexpr OctreePoint point = { 0.0f, 0.0f, 0.0f };

	GLuint vao, buffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(point),
				 &point, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreePoint), 0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ctx.point_vao = vao;
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

	move_delta /= 8.0f;

	ctx.sensor.position += move_delta;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ctx.shader.id);

	mat4 matrix;

	float aspect = (float)frame.window.width/(float)frame.window.height;
	matrix = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);
	matrix = glm::rotate(matrix, PI/2 * ctx.camera.pitch, vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, PI*2 * ctx.camera.yaw,   vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::translate(matrix, -ctx.camera.position);

	glUniformMatrix4fv(ctx.shader.in_projection_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	matrix = mat4(1.0f);
	matrix = glm::translate(matrix, vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	glBindVertexArray(ctx.mdl.vao);

	// glPointSize(5.0f);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glUniform3f(ctx.shader.diffuse_color, 1.0f, 1.0f, 1.0f);
	// glUniform3f(ctx.shader.emission_color, 0.0f, 0.0f, 0.0f);
	// glDrawArrays(GL_POINTS, 0, ctx.mdl.num_vertex);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUniform3f(ctx.shader.diffuse_color, 1.0f, 1.0f, 1.0f);
	glUniform3f(ctx.shader.emission_color, 0.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, ctx.mdl.num_vertex);
	glBindVertexArray(0);

	// glPointSize(5.0f);
	// glBindVertexArray(ctx.point_vao);

	matrix = mat4(1.0f);
	matrix = glm::translate(matrix, ctx.sensor.position);
	matrix = glm::scale(matrix, vec3(0.1f));
	glUniformMatrix4fv(ctx.shader.in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	// glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	// glUniform3f(ctx.shader.emission_color, 1.0f, 0.0f, 0.0f);
	// glDrawArrays(GL_POINTS, 0, 1);
	// glBindVertexArray(0);

	glBindVertexArray(ctx.sphere.vao);
	glUniform3f(ctx.shader.diffuse_color, 1.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 0.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLES, 0, ctx.sphere.num_vertex);
	glBindVertexArray(0);

	glUseProgram(0);
}
