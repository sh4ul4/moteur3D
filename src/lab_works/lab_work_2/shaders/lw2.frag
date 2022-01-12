#version 450

layout( location = 0) out vec4 fragColor;
in vec4 outColor;

uniform float lightIntensity = 1;

void main() {
	fragColor = outColor;
	fragColor.x *= lightIntensity;
	fragColor.y *= lightIntensity;
	fragColor.z *= lightIntensity;
}
