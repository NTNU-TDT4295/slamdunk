#include "model_loader_obj.h"
#include "my_string.h"
#include "config_parser.h"
#include "linmath.h"
#include "utils.h"
#include "opengl.h"
#include <stdio.h>

struct obj_vertex {
	vec3 v;
	vec3 vn;
	vec2 vt;

	struct {
		unsigned int v;
		unsigned int vt;
		unsigned int vn;
	} f[3];
};

bool load_obj_file(arena transient, string filename, model *out) {
	struct config_file cfg = {};
	string filename_zeroterm;
	size_t n_alloced = 0;
	size_t n_vertex = 0;
	size_t n_vertex_normal = 0;
	size_t n_face = 0;
	obj_vertex *first_node = 0;

	filename_zeroterm = string_duplicate_zeroterminate(&transient, filename);

	cfg.file = filename;
	cfg.fd = fopen((char *)filename_zeroterm.data, "rb");

	while (config_next_line(&cfg)) {
		string token;

		config_eat_token(&cfg, &token);

		if (token == STR("o")) {
			string name;
			config_eat_token(&cfg, &name);

			print_error("obj", "object: %.*s\n", LIT(name));
		} else if (token == STR("v")) {
			obj_vertex *vtx;
			float x, y, z;

			if (n_vertex >= n_alloced) {
				vtx = arena_alloc<obj_vertex>(&transient);
				if (n_alloced == 0) {
					first_node = vtx;
				}
				n_alloced += 1;
			} else {
				vtx = &first_node[n_vertex];
			}
			n_vertex += 1;

			if (!config_eat_token_float(&cfg, &x)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}
			if (!config_eat_token_float(&cfg, &y)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}
			if (!config_eat_token_float(&cfg, &z)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}

			vtx->v = vec3(x, y, z);
		} else if (token == STR("vn")) {
			obj_vertex *vtx;
			float x, y, z;

			if (n_vertex_normal >= n_alloced) {
				vtx = arena_alloc<obj_vertex>(&transient);
				if (n_alloced == 0) {
					first_node = vtx;
				}
				n_alloced += 1;
			} else {
				vtx = &first_node[n_vertex_normal];
			}
			n_vertex_normal += 1;

			if (!config_eat_token_float(&cfg, &x)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}
			if (!config_eat_token_float(&cfg, &y)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}
			if (!config_eat_token_float(&cfg, &z)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}

			vtx->vn = vec3(x, y, z);
		} else if (token == STR("f")) {
			obj_vertex *vtx;
			uint64_t remaining_tokens;

			if (n_face >= n_alloced) {
				vtx = arena_alloc<obj_vertex>(&transient);
				if (n_alloced == 0) {
					first_node = vtx;
				}
				n_alloced += 1;
			} else {
				vtx = &first_node[n_face];
			}
			n_face += 1;

			for (int i = 0; i < 3; i++) {
				string token;
				string subtoken;
				uint64_t id;

				if (!config_eat_token(&cfg, &token)) {
					print_error("obj load",
								"Missing %i parameters from f (face) statement.", (3 - i));
					break;
				}
				string_split(token, &subtoken, &token, '/');
				if (string_to_uint64(subtoken, &id)) {
					vtx->f[i].v = id;
				} else {
					print_error("obj load", "Missing vertex index for f (face) statement.");
				}

				string_split(token, &subtoken, &token, '/');
				if (string_to_uint64(subtoken, &id)) {
					vtx->f[i].vt = id;
				}

				string_split(token, &subtoken, &token, '/');
				if (string_to_uint64(subtoken, &id)) {
					vtx->f[i].vn = id;
				}
			}

			remaining_tokens = config_count_remaining_tokens(&cfg);
			if (remaining_tokens > 0) {
				print_error("obj load",
							"Too many arguments to f (face) statement. Expected 3, got %i. "
							"We currently only support triangulated shapes.",
							3 + remaining_tokens);
			}
		}
	}

	assert((transient.capacity - transient.head) >= (sizeof(float) * 8) * 3 * n_face);

	float *vertex_data;
	size_t vertex_count = 3 * n_face;
	size_t vertex_size = 8;

	vertex_data = (float *)calloc(vertex_count * vertex_size, sizeof(float));

	for (size_t i = 0; i < n_face; i++) {
		obj_vertex *tex = &first_node[i];
		size_t face_data_begin = i*vertex_size*3;

		for (int j = 0; j < 3; j++) {
			size_t vertex_data_begin = face_data_begin + j * vertex_size;
			if (tex->f[j].v > 0) {
				obj_vertex *vertex = &first_node[tex->f[j].v - 1];

				vertex_data[vertex_data_begin+0] = vertex->v.x;
				vertex_data[vertex_data_begin+1] = vertex->v.y;
				vertex_data[vertex_data_begin+2] = vertex->v.z;
			} else {
				print_error("obj load", "Missing vertex position info.");
			}

			if (tex->f[j].vn > 0) {
				obj_vertex *vertex_normal = &first_node[tex->f[j].vn - 1];
				vertex_data[vertex_data_begin+3] = vertex_normal->vn.x;
				vertex_data[vertex_data_begin+4] = vertex_normal->vn.y;
				vertex_data[vertex_data_begin+5] = vertex_normal->vn.z;
			} else {
				print_error("obj load", "Missing normal");
			}

			if (tex->f[j].vt > 0) {
				obj_vertex *vertex_texture = &first_node[tex->f[j].vt - 1];
				vertex_data[vertex_data_begin+6] = vertex_texture->vt.x;
				vertex_data[vertex_data_begin+7] = vertex_texture->vt.y;
			}
		}
	}

	GLuint vao, buffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER,
				 vertex_count*vertex_size*sizeof(float),
				 vertex_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0*sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	out->vao = vao;
	out->vbo = buffer;
	out->vertices = vertex_data;
	out->num_vertex = n_face * 3;

	return true;
}
