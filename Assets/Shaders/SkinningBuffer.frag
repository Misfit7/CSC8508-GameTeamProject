#version 400 core

uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;
uniform int emissive; 
uniform bool hasTexture;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;
uniform vec3	cameraPos;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour[5];

void main(void)
{
	float shadow = 1.0; // New !
	
	if( IN . shadowProj . w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}
	
	vec4 albedo = IN.colour;
	if(hasTexture) {
	 albedo *= texture(mainTex, IN.texCoord);
	}

    fragColour [0] = albedo;
    fragColour [1] = vec4(IN.normal.xyz * 0.5 + 0.5 ,1.0);
    fragColour [2] = vec4(0.2,0.2,0.2,1.0);
    fragColour [3] = vec4(emissive,1.0,1.0,1.0);
    fragColour [4] = vec4(shadow, 1.0, 1.0, 1.0); 
}
