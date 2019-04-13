#version 430 core

vec3 CUBE_VERTICES[] = {
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

vec3 CUBE_NORMALS[] = {
	// xy -z
	{0.0f, 0.0f, -1.0f},
	// xy +z
	{0.0f, 0.0f, 1.0f},
	// yz -x
	{-1.0f, 0.0f, 0.0f},
	// yz +x
	{1.0f, 0.0f, 0.0f},
	// xz -y
	{0.0f, -1.0f, 0.0f},
	// xz +y
	{0.0f, 1.0f, 0.0f},
};

// reduce to 4 in all files
uniform mat4 chunk;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout (points) in;
layout (triangle_strip, max_vertices=6) out;

in VS_OUT {
	vec3 texcoords[4];
	float face;
} gs_in[];

out vec3 texcoord;
out vec3 frag_pos;
out vec3 normal;

vec4 transform(vec4 v){
	return proj * view * model * chunk * v;
}

void main() {
    normal = CUBE_NORMALS[int(gs_in[0].face)];

	for(int f = 0; f < 4; f++){
		vec4 corner = gl_in[0].gl_Position + vec4(CUBE_VERTICES[int(gs_in[0].face)*4+f], 0);
		gl_Position = transform(corner);
		texcoord = gs_in[0].texcoords[f];
		EmitVertex();
	}
//	for(int i = 0; i < 3; i++){
//		gl_Position = gl_in[i].gl_Position;
//		texcoord = gs_in[i].texcoord;
//		frag_pos = gs_in[i].frag_pos;
//		normal = normalize(cross(vec3(gs_in[0].frag_pos - gs_in[2].frag_pos), 
//		vec3(gs_in[1].frag_pos - gs_in[0].frag_pos)));
//		EmitVertex();
//	}
}