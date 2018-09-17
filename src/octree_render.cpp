#include "octree_render.h"
#include "opengl.h"
#include "linmath.h"
#include <stdio.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static size_t octree_collect_nodes(OctreeNode *node, OctreePoint *points, size_t cap_points) {
	if (!node) {
		return 0;
	}

	assert(cap_points >= node->num_points);

	if (node->num_points >= OCTREE_LEAFS) {
		size_t points_collected = 0;
		for (size_t i = 0; i < OCTREE_CHILDREN; i++) {
			assert(cap_points >= points_collected);
			points_collected += octree_collect_nodes(node->children[i],
													 &points[points_collected],
													 cap_points - points_collected);
		}

		assert(points_collected == node->num_points);
	} else {
		for (size_t i = 0; i < node->num_points; i++) {
			points[i] = node->leafs[i];
		}
	}

	return node->num_points;
}

static void octree_render_update_node(OctreeRender &ctx, OctreeNode *node) {
	if (!node) {
		return;
	}

	if (!node->vao_id) {
		glGenVertexArrays(1, &node->vao_id);
		node->dirty = true;
	}

	glBindVertexArray(node->vao_id);

	if (!node->buffer_id) {
		glGenBuffers(1, &node->buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, node->buffer_id);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreePoint), 0);
		glEnableVertexAttribArray(0);

		node->dirty = true;

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (node->dirty) {
		OctreePoint *points = (OctreePoint *)calloc(node->num_points, sizeof(OctreePoint));
		octree_collect_nodes(node, points, node->num_points);
		for (size_t i = 0; i < node->num_points; i++) {
			printf("%f %f %f\n", points[i].x, points[i].y, points[i].z);
		}

		glBindBuffer(GL_ARRAY_BUFFER, node->buffer_id);
		glBufferData(GL_ARRAY_BUFFER, node->num_points * sizeof(OctreePoint),
					points, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		free(points);
		node->dirty = false;
	}

	mat4 matrix = mat4(1.0f);
	glUniformMatrix4fv(ctx.matrix_uniform, 1, GL_FALSE, glm::value_ptr(matrix));

	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, node->num_points);

	glBindVertexArray(0);
}

constexpr OctreePoint cube_points[] = {
	// Front face
	{ -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f },

	// Back face
	{ -1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f },

	{ -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f,  1.0f },
	{  1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f,  1.0f },
};
constexpr size_t cube_points_length = sizeof(cube_points) / sizeof(OctreePoint);

void octree_render_init(OctreeRender &ctx) {

	GLuint vao, buffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points),
				 cube_points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OctreePoint), 0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ctx.cube_vao = vao;
}

static void octree_render_bounds_internal(OctreeRender &ctx, OctreeNode *node) {
	if (!node) {
		return;
	}

	mat4 matrix = mat4(1.0f);
	matrix = glm::translate(matrix, vec3(node->box.center.x,
										 node->box.center.y,
										 node->box.center.z));
	matrix = glm::scale(matrix, vec3(node->box.radius));
	glUniformMatrix4fv(ctx.matrix_uniform, 1, GL_FALSE, glm::value_ptr(matrix));

	glBindVertexArray(ctx.cube_vao);
	glDrawArrays(GL_LINES, 0, cube_points_length);

	if (node->num_points >= OCTREE_LEAFS) {
		for (size_t i = 0; i < OCTREE_CHILDREN; i++) {
			octree_render_bounds_internal(ctx, node->children[i]);
		}
	}
}

void octree_render_bounds(OctreeRender &ctx) {
	octree_render_bounds_internal(ctx, ctx.octree.root);
}

void octree_render(OctreeRender &ctx) {
	octree_render_update_node(ctx, ctx.octree.root);
}
