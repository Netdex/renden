#version 430 core

//uniform mat4 chunk;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 proj;

in ivec3 position;
in int face;
in uint tex_offset;
//in vec3 texcoords[4];

out VS_OUT {
//	vec3 frag_pos;
//	vec3 texcoords[4];
	uint face;
	uint tex_offset;
} vs_out;

void main()
{
//    vs_out.texcoords = texcoords;
	vs_out.tex_offset = tex_offset;
	vs_out.face = face;
	gl_Position = vec4(position, 1.0);
//    gl_Position = proj * view * model * chunk * vec4(position, 1.0);
//	vs_out.frag_pos = vec3(model * chunk * vec4(position, 1.0));
}