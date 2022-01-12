#version 450

layout( location = 0 ) in vec2 aVertexPosition;
in vec4 inColor;
out vec4 outColor;

uniform float uTranslationX;

void main() {
	gl_Position = vec4(aVertexPosition ,0.0f, 1.0f);
	gl_Position.x += uTranslationX;
	outColor = inColor;
}
