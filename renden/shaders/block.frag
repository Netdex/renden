#version 430 core

uniform sampler2DArray tex;

in vec3 texcoord;
in vec3 frag_pos;
in vec3 normal;

out vec4 out_color;

void main() {
    vec4 texColor = texture(tex, texcoord);
    if(texColor.a < 0.1)
        discard;

	vec3 lightColor = vec3(1,1,1);
	vec3 norm = normal;
	vec3 lightPos = vec3(128,256,128);
	vec3 lightDir = normalize(lightPos - frag_pos);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = diff * lightColor;
	vec3 ambient = 0.5 * lightColor;
	out_color = vec4(diffuse + ambient, 1.0) * texColor;
}
