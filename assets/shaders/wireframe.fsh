#version 330

in vec3 fin_position;
in vec3 fin_normal;

layout(location=0) out vec4 out_color;

uniform vec3 line_color;

void main() {
	out_color = line_color;
}
