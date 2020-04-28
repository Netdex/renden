#version 430 core

#define FACE_NEG_Z	(1 << 0)
#define FACE_POS_Z	(1 << 1)
#define FACE_NEG_X	(1 << 2)
#define FACE_POS_X	(1 << 3)
#define FACE_NEG_Y	(1 << 4)
#define FACE_POS_Y	(1 << 5)

const int kFaceToBlock[] = {FACE_NEG_Z, FACE_POS_Z, FACE_NEG_X, FACE_POS_X, FACE_NEG_Y, FACE_POS_Y};

const vec3 CUBE_VERTICES[] = {
	// xy -z
	{0.5f, -0.5f, -0.5f},
	{0.5f, 0.5f, -0.5f},
	{-0.5f, -0.5f, -0.5f},
	{-0.5f, 0.5f, -0.5f},
	// xy +z
	{-0.5f, -0.5f, 0.5f},
	{-0.5f, 0.5f, 0.5f},
	{0.5f, -0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	// yz -x
	{-0.5f, 0.5f, -0.5f},
	{-0.5f, 0.5f, 0.5f},
	{-0.5f, -0.5f, -0.5f},
	{-0.5f, -0.5f, 0.5f},
	// yz +x
	{0.5f, -0.5f, -0.5f},
	{0.5f, -0.5f, 0.5f},
	{0.5f, 0.5f, -0.5f},
	{0.5f, 0.5f, 0.5f},
	// xz -y
	{-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, 0.5f},
	{0.5f, -0.5f, -0.5f},
	{0.5f, -0.5f, 0.5f},
	// xz +y
	{0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, 0.5f},
	{-0.5f, 0.5f, -0.5f},
	{-0.5f, 0.5f, 0.5f},
};

const vec3 CUBE_NORMALS[] = {
	{0.0f, 0.0f, -1.0f},    // xy -z
	{0.0f, 0.0f, 1.0f},	    // xy +z
	{-1.0f, 0.0f, 0.0f},	// yz -x
	{1.0f, 0.0f, 0.0f},	    // yz +x
	{0.0f, -1.0f, 0.0f},	// xz -y
	{0.0f, 1.0f, 0.0f},	    // xz +y
};

// Must agree with other definitions
const int SHADOW_MAP_PARTITIONS = 3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 shadow_view[SHADOW_MAP_PARTITIONS];
uniform mat4 shadow_proj[SHADOW_MAP_PARTITIONS];
//uniform float now;

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

// Invoke 6 times per vertex for every face
layout (invocations = 6) in;

in VS_OUT {
	uint face;
	uint tex_offset;
} gs_in[];

out vec3 texcoord;
out vec3 frag_pos;
out vec3 normal;
out vec4 shadow_frag_pos[SHADOW_MAP_PARTITIONS];

uniform usampler1D str_sampler;

void main() {
	
	// Do we need to draw this face?
	if((kFaceToBlock[gl_InvocationID] & gs_in[0].face) != 0){
		for(int f = 0; f < 4; f++){
			vec4 corner = gl_in[0].gl_Position + vec4(CUBE_VERTICES[gl_InvocationID*4+f], 0);
			vec4 world_loc = model * corner;

			gl_Position = proj * view * world_loc;
			texcoord = vec3(texelFetch(str_sampler, int(gs_in[0].tex_offset*(6*4) + gl_InvocationID*4+f), 0));
			frag_pos = vec3(world_loc);
			normal = CUBE_NORMALS[gl_InvocationID];
			for(int i = 0; i < SHADOW_MAP_PARTITIONS; ++i){
				shadow_frag_pos[i] = shadow_proj[i] * shadow_view[i] * world_loc;
			}
			EmitVertex();
		}
	}
}

