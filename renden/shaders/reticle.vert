#version 430 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 position;
in vec3 color;

out vec3 frag_color;

void main()
{
    gl_Position = proj * view * model * vec4(position, 1.0);
	frag_color = color;
}