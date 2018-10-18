#version 330

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec2 in_uv;

out vec3 fin_position;
out vec3 fin_normal;
out vec2 fin_uv;

uniform mat4 in_matrix;
uniform mat4 in_projection_matrix;

void main() {
	fin_normal = (transpose(inverse(in_matrix)) * vec4(in_normal, 0.0)).xyz;
	fin_position = (in_matrix * vec4(in_position, 1.0)).xyz;
	fin_uv = in_uv;
	gl_Position = in_projection_matrix * in_matrix * vec4(in_position, 1.0);
}
