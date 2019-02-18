#version 150 core

uniform sampler2DArray tex;

in vec3 Texcoord;

out vec4 outColor;

void main() {
	outColor = texture(tex, Texcoord);
}
