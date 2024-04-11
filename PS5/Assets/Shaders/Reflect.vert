#version 400 core

uniform mat4 modelMatrix 	  = mat4(1.0f);
uniform mat4 viewMatrix 	  = mat4(1.0f);
uniform mat4 projMatrix       = mat4(1.0f);
uniform mat4 shadowMatrix 	  = mat4(1.0f);
uniform mat4 textureMatrix    = mat4(1.0f); 
uniform vec4 objectColour     = vec4(1,1,1,1);
uniform bool hasVertexColours = false;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec4 tangent;

out Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 tangent;
    vec3 binormal;
	vec3 normal;
	vec3 worldPos;
} OUT;

void main(void)
{
	mat4 mvp 		  = (projMatrix * viewMatrix * modelMatrix);
	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));
	
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
    vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.shadowProj 	=  shadowMatrix * vec4 ( position,1);
	OUT.worldPos 	= ( modelMatrix * vec4 ( position ,1)). xyz ;
	
	OUT.normal = wNormal;
    OUT.tangent = wTangent;
    OUT.binormal = cross(wTangent, wNormal) * tangent.w;
	
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.colour	 = objectColour;

	if(hasVertexColours) {
		OUT.colour		= objectColour * colour;
	}
	gl_Position		= mvp * vec4(position, 1.0);
}
