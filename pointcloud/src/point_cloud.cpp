#include "point_cloud.h"

#include "opengl.h"
#include "utils.h"
#include "octree_loader.h"

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

	octree_render_init(ctx.octree_render);
	ctx.octree_render.matrix_uniform = ctx.shader.in_matrix;


	Octree *octree = &ctx.octree_render.octree;
	octree->box.center = { 0.0f, 0.0f, 0.0f };
	octree->box.radius = 1000.0f;

	octree_load_obj(octree, "assets/models/mountain.obj");
}

void tick_point_cloud(PointCloudContext &ctx, const WindowFrameInfo &frame) {
	update_camera(ctx.camera, frame);

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

	glUseProgram(0);
}
