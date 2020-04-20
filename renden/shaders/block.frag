#version 430 core

uniform sampler2DArray tex;
uniform sampler2DArray shadow_map;
uniform vec3 camera_pos;

in vec3 texcoord;
in vec3 frag_pos;
in vec3 normal;
in vec4 shadow_frag_pos;

out vec4 out_color;

const vec3 LIGHT_COLOR      = vec3(1,1,1);
const vec3 LIGHT_DIR        = vec3(1,1,0.1);
const vec3 LIGHT_POS		= vec3(128,256,128);

const float SPECULAR_STR    = 0.5;
const float AMBIENT_STR     = 0.5;
const float DIFFUSE_STR     = 0.5;

float shadow_intensity(vec4 frag_pos){
	// Not necessary for orthographic light space projection.
	vec3 proj_coords = frag_pos.xyz / frag_pos.w;
	proj_coords = proj_coords * 0.5 + 0.5;
//	float closest_depth = texture(shadow_map, proj_coords.xy).r;
	float bias = 0;
//	return proj_coords.z - bias > closest_depth ? 1.0 : 0.0;
	float shadow = 0.0;
	vec2 texel_size = (1.0 / textureSize(shadow_map, 0)).xy;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcf_depth = texture(shadow_map, vec3(proj_coords.xy + vec2(x, y) * texel_size,0)).r; 
			shadow += proj_coords.z - bias > pcf_depth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;
}

void main() {
    vec4 tex_color = texture(tex, texcoord);
    if(tex_color.a < 0.1)
        discard;

	vec3 light_dir = normalize(LIGHT_POS - frag_pos);
//    vec3 light_dir = LIGHT_DIR;
	float diff = max(dot(normal, LIGHT_DIR), 0.0);
	vec3 diffuse = DIFFUSE_STR * diff * LIGHT_COLOR;

	vec3 ambient = AMBIENT_STR * LIGHT_COLOR;

	vec3 view_dir = normalize(camera_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);

	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = SPECULAR_STR * spec * LIGHT_COLOR;

	float shadow = shadow_intensity(shadow_frag_pos);

	out_color = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0) * tex_color;
}
