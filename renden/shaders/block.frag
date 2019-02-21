#version 150 core

uniform sampler2DArray tex;

in vec3 Texcoord;

out vec4 outColor;

void main() {
    vec4 texColor = texture(tex, Texcoord);
    if(texColor.a < 0.1)
        discard;
	outColor = texColor;
}
