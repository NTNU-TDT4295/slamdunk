#include "point_cloud.h"

#include "opengl.h"
#include "utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	arena transient;

	arena_init(&transient, MEGABYTE(1));
	if (!load_obj_file(transient, STR("assets/models/room.obj"), &ctx.mdl)) {
		panic("Failed to load model!");
	}
	arena_free(&transient);

	octree_render_init(ctx.octree_render);
	ctx.octree_render.matrix_uniform = ctx.shader.in_matrix;
}

static void update_camera(PointCloudContext &ctx, const WindowFrameInfo &frame) {
	if (frame.window.dimentions_changed) {
		// float aspect = (float)frame.window.width/(float)frame.window.height;

		// mat4 projection_matrix;
		// projection_matrix = glm::perspective(45.0f, aspect, 0.01f, 100.0f);

		// glUseProgram(ctx.shader.id);
		// glUniformMatrix4fv(ctx.shader.in_projection_matrix, 1, GL_FALSE,
		// 				   glm::value_ptr(projection_matrix));
		// glUseProgram(0);
	}

	ctx.camera.yaw += (float)frame.mouse.dx * 0.005f;
	ctx.camera.yaw -= floor(ctx.camera.yaw);

	ctx.camera.pitch += (float)frame.mouse.dy * 0.005f;
	if (ctx.camera.pitch > 1.0f) {
		ctx.camera.pitch = 1.0f;
	} else if (ctx.camera.pitch < -1.0f) {
		ctx.camera.pitch = -1.0f;
	}

	vec3 move_delta = {};
	vec3 move_final = {};

	if (frame.keyboard.left)     {move_delta.x -= 1.0f;}
	if (frame.keyboard.right)    {move_delta.x += 1.0f;}

	if (frame.keyboard.forward)  {move_delta.z -= 1.0f;}
	if (frame.keyboard.backward) {move_delta.z += 1.0f;}

	move_delta /= 0.5;

	if (move_delta.x != 0.0f) {
		move_final.x += cos(PI*2*ctx.camera.yaw)/move_delta.x;
		move_final.z += sin(PI*2*ctx.camera.yaw)/move_delta.x;
	}

	if (move_delta.z != 0.0f) {
		move_final.x += -sin(PI*2*ctx.camera.yaw)/move_delta.z;
		move_final.z += cos(PI*2*ctx.camera.yaw)/move_delta.z;
		move_final.y += sin((PI/2.0f)*ctx.camera.pitch)/move_delta.z;
	}

	ctx.camera.position += move_final * (frame.keyboard.accelerate ? 0.5f : 0.1f);

}

void tick_point_cloud(PointCloudContext &ctx, const WindowFrameInfo &frame) {
	update_camera(ctx, frame);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(ctx.shader.id);

	mat4 matrix;

	float aspect = (float)frame.window.width/(float)frame.window.height;
	matrix = glm::perspective(45.0f, aspect, 0.01f, 100.0f);
	matrix = glm::rotate(matrix, PI/2 * ctx.camera.pitch, vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, PI*2 * ctx.camera.yaw,   vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::translate(matrix, -ctx.camera.position);

	glUniformMatrix4fv(ctx.shader.in_projection_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	// glLineWidth(3.0f);
	glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 0.0f, 1.0f, 1.0f);
	octree_render_bounds(ctx.octree_render);

	glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	glUniform3f(ctx.shader.emission_color, 1.0f, 0.0f, 0.0f);
	octree_render(ctx.octree_render);

	// matrix = mat4(1.0f);
	// matrix = glm::translate(matrix, vec3(0.0f, 0.0f, 0.0f));
	// glUniformMatrix4fv(ctx.octree_render.matrix_uniform, 1, GL_FALSE, glm::value_ptr(matrix));

	// glBindVertexArray(ctx.mdl.vao);

	// glPointSize(5.0f);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glUniform3f(ctx.shader.diffuse_color, 1.0f, 1.0f, 1.0f);
	// glUniform3f(ctx.shader.emission_color, 0.0f, 0.0f, 0.0f);
	// glDrawArrays(GL_POINTS, 0, ctx.mdl.num_vertex);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glUniform3f(ctx.shader.diffuse_color, 0.0f, 0.0f, 0.0f);
	// glUniform3f(ctx.shader.emission_color, 0.0f, 1.0f, 1.0f);
	// glDrawArrays(GL_TRIANGLES, 0, ctx.mdl.num_vertex);
	// glBindVertexArray(0);

	glUseProgram(0);
}
