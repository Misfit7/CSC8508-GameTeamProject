#version 330 core

uniform sampler2D diffuseTex;

out vec4 fragColour;

in Vertex{
    vec2 texCoord;
} IN;


void main(void) {
    fragColour = texture(diffuseTex, IN.texCoord);
}