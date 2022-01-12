#version 450

out vec4 fragColor;

in vec2 TexCoords;

uniform sampler2D bloomBlur;
uniform sampler2D scene;
uniform bool bloom;
uniform float exposure;

void main()
{             
    // high dynamic range pixel value
    vec3 hdrColor = texture(scene, TexCoords).rgb;

    // bloom-light to add
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    
    // perform bloom additions and compute result color
    if(bloom)
        hdrColor += bloomColor;
    vec3 res = vec3(1.0) - exp(-hdrColor * 1.4);

    // gamma correction
    const float gamma = 0.8;
    res = pow(res, vec3(1.0 / gamma));
    fragColor = vec4(res, 1.0);
}