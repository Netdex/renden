#version 430 core

//#define DEBUG_SHADOW_PARTITION

// Must agree with other definitions
const int SHADOW_MAP_PARTITIONS = 3;

const vec3 LIGHT_COLOR      = vec3(1,1,1);

const float SPECULAR_STR    = 0.3;
const float AMBIENT_STR     = 0.5;
const float DIFFUSE_STR     = 0.5;

uniform sampler2DArray tex;
uniform sampler2DArrayShadow shadow_map;
uniform vec3 camera_pos;
uniform vec3 light_dir;
//uniform float shadow_depth[SHADOW_MAP_PARTITIONS];

in vec3 texcoord;
in vec3 frag_pos;
in vec3 normal;
in vec4 shadow_frag_pos[SHADOW_MAP_PARTITIONS];

out vec4 out_color;

vec2 shadow_intensity(){
	float bias = 0;

	vec2 texel_size = 1.0 / textureSize(shadow_map, 0).xy;
	for(int i = 0; i < SHADOW_MAP_PARTITIONS; ++i){
		// Not necessary for orthographic light space projection.
		vec3 proj_coords = shadow_frag_pos[i].xyz / shadow_frag_pos[i].w;
		proj_coords = proj_coords * 0.5 + 0.5;
		
		if(clamp(proj_coords.x, 0, 1) == proj_coords.x 
				&& clamp(proj_coords.y, 0, 1) == proj_coords.y
				&& clamp(proj_coords.z, 0, 1) == proj_coords.z){
			float shadow = dot(textureGather(shadow_map, vec3(proj_coords.xy, i), proj_coords.z), vec4(1)) / 4;
			return vec2(shadow, i);
		}
	}
	return vec2(0, SHADOW_MAP_PARTITIONS);
}

void main() {
    vec4 tex_color = texture(tex, texcoord);
    if(tex_color.a < 0.1)
        discard;

	//vec3 light_to_frag = normalize(LIGHT_POS - frag_pos);
//    vec3 light_dir = LIGHT_DIR;
	float diff = max(dot(normal, -light_dir), 0.0);
	vec3 diffuse = DIFFUSE_STR * diff * LIGHT_COLOR;

	vec3 ambient = AMBIENT_STR * LIGHT_COLOR;

	vec3 view_dir = normalize(camera_pos - frag_pos);
	vec3 reflect_dir = reflect(light_dir, normal);

	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = SPECULAR_STR * spec * LIGHT_COLOR;

	vec2 shadow = shadow_intensity();

	vec3 intensity = ambient + (shadow.r) * (diffuse + specular);
#ifdef DEBUG_SHADOW_PARTITION
	vec3 alt_color[SHADOW_MAP_PARTITIONS+1] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), vec3(0,0,0)};
	intensity += 0.2 * alt_color[int(shadow.g)];
#endif
	out_color = vec4(intensity, 1.0) * tex_color;
}
