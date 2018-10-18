#version 330

in vec3 fin_position;
in vec3 fin_normal;
in vec2 fin_uv;

uniform sampler2D tex;

layout(location=0) out vec4 out_color;

void main() {
	float v = texture(tex, fin_uv).x;
	out_color = vec4(v, v, v, 1.0);
	// out_color = vec4(fin_uv, 0.0, 1.0);
}
