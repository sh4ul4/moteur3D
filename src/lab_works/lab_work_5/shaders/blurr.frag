#version 450

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;

// constant weight values
uniform float w[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {             
    vec2 offset = 1.0 / textureSize(image, 0);
    vec3 res = texture(image, TexCoords).rgb * w[0];
    // horizontal blurring
    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
            res += texture(image, TexCoords + vec2(offset.x * i, 0.0)).rgb * w[i];
            res += texture(image, TexCoords - vec2(offset.x * i, 0.0)).rgb * w[i];
        }
    }
    // vertical blurring
    else {
        for(int i = 1; i < 5; ++i) {
            res += texture(image, TexCoords + vec2(0.0, offset.y * i)).rgb * w[i];
            res += texture(image, TexCoords - vec2(0.0, offset.y * i)).rgb * w[i];
        }
    }
    FragColor = vec4(res, 1.0);
}