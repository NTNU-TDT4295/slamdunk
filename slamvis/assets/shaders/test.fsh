#version 330

in vec3 fin_position;
in vec3 fin_normal;

uniform vec3 emission_color;
uniform vec3 diffuse_color;

layout(location=0) out vec4 out_color;

void main() {
	// out_color = vec4(abs(fin_normal) + vec3(0.1), 1.0);
	float brightness = dot(normalize(fin_normal), normalize(vec3(-1.0, 0.4, 0.9)));
	brightness = max(brightness, 0.2);
	out_color = vec4(diffuse_color * brightness, 1) + vec4(emission_color, 1.0);
}
