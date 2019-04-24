#version 440

uniform mat4 ModelM;
uniform mat4 ProjM;
uniform mat4 CameraM;

out vec4 FragColor;
in vec3 color;

void main()
{
	FragColor = vec4(vec3(abs(color[0]), abs(color[1]), abs(color[2]))/20, 1.0);
	//FragColor = vec4(1.0,0.0,0.0,1.0);
}