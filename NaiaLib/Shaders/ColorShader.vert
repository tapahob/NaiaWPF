#version 400 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;


void main()
{
	gl_Position = vec4(Position, 1.0);
}