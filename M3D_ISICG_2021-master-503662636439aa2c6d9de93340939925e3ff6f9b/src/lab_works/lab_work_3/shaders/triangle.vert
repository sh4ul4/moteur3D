#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexColor;

out vec3 color;

void main()
{
	color = aVertexColor;

	gl_Position = vec4( aVertexPosition, 1.f );
}
