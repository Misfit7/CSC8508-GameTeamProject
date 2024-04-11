#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 modelMatrix 	  = mat4(1.0f);
uniform mat4 viewMatrix 	  = mat4(1.0f);
uniform mat4 projMatrix		  = mat4(1.0f);
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = texCoord;
    ParticleColor = color;
    //gl_Position = modelMatrix*viewMatrix*projMatrix * vec4((position * scale) + offset, 1.0);
    //gl_Position = modelMatrix*viewMatrix*projMatrix * vec4(position, 1.0);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4((position + offset) * scale, 1.0);
}