#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uMVMatrix;
uniform mat4 uMMatrix;
uniform mat4 uNormalMatrix;

out vec3 Norm;
out vec3 FragPosView;
out vec3 Lpos;

void main()
{
    Norm = normalize(uNormalMatrix * vec4(aVertexNormal, 0.f )).xyz;
    FragPosView = (uMVMatrix * vec4(aVertexPosition, 1.f)).xyz;
    vec3 modelpos = (uMVMatrix * vec4(aVertexPosition, 1.f)).xyz;
    vec3 lightpos = (uMVMatrix * vec4(450.f,300.f,-150.f, 1.f)).xyz;
    Lpos = normalize(lightpos - modelpos);
    gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
}
