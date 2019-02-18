#version 150 core

uniform mat4 chunk;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 position;
in vec3 texcoord;

out vec3 Texcoord;

void main()
{
    Texcoord = texcoord;
    gl_Position = proj * view * model * chunk * vec4(position, 1.0);
}