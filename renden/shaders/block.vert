#version 430 core

//uniform mat4 chunk;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 proj;

layout(location = 0) in lowp ivec3 position;
layout(location = 1) in lowp int face;
layout(location = 2) in lowp vec3 texcoords[4];

out VS_OUT {
//	vec3 frag_pos;
	vec3 texcoords[4];
	int face;
} vs_out;

void main()
{
    vs_out.texcoords = texcoords;
	vs_out.face = face;
	gl_Position = vec4(position, 1.0);
//    gl_Position = proj * view * model * chunk * vec4(position, 1.0);
//	vs_out.frag_pos = vec3(model * chunk * vec4(position, 1.0));
}