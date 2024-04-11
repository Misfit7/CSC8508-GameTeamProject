#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;

layout (location = 0) out vec4 fragColor;
//layout (location = 5) out vec4 BrightColor;

uniform sampler2D sprite;

void main()
{
    fragColor = (texture(sprite, TexCoords) * ParticleColor);
	//BrightColor = (texture(sprite, TexCoords) * ParticleColor);
}