#version 430 core
out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube tex;

void main()
{
    FragColor = texture(tex, TexCoord);
}