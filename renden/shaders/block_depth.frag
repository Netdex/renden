#version 430 core

out vec4 out_color;
void main()
{
	// It's lonely in here.
	out_color = vec4(vec3(gl_FragCoord.z / gl_FragCoord.w), 1);
}