#version 330 core
in vec3 position;

out vec3 TexCoord;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

void main()
{
    TexCoord = position;
    vec4 pos = proj * view * model * vec4(position, 1.0);
    gl_Position = pos.xyww;
}