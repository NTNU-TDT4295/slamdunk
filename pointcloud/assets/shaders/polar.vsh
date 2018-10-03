#version 330

#define M_PI 3.1415926535897932384626433832795

layout(location=0) in vec2 in_coord;

uniform float scale;

void main() {
	vec2 pos;
	float angle = in_coord.x * M_PI / 180.0;
	pos = vec2(cos(angle), sin(angle));
	pos *= in_coord.y / scale;
	gl_Position = vec4(pos, 0.0, 1.0);
}
