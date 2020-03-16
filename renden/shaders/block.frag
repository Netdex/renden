#version 430 core

uniform sampler2DArray tex;
uniform vec3 camera_pos;

in vec3 texcoord;
in vec3 frag_pos;
in vec3 normal;

out vec4 out_color;

const vec3 LIGHT_COLOR      = vec3(1,1,1);
const vec3 LIGHT_DIR        = vec3(1,1,0.5);
const vec3 LIGHT_POS		= vec3(128,256,128);

const float SPECULAR_STR    = 0.0;
const float AMBIENT_STR     = 0.5;
const float DIFFUSE_STR     = 0.5;

void main() {
    vec4 tex_color = texture(tex, texcoord);
    if(tex_color.a < 0.1)
        discard;

//	vec3 light_dir = normalize(LIGHT_POS - frag_pos);
    vec3 light_dir = LIGHT_DIR;
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = DIFFUSE_STR * diff * LIGHT_COLOR;

	vec3 ambient = AMBIENT_STR * LIGHT_COLOR;

	vec3 view_dir = normalize(camera_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);

	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = SPECULAR_STR * spec * LIGHT_COLOR;

	out_color = vec4(diffuse + ambient + specular, 1.0) * tex_color;
}
