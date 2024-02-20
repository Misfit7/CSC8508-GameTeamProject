#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	float shadow = 1.0; // New !
	
	if( IN.shadowProj.w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}

	//diffuse
	vec3  lightDir = normalize ( lightPos - IN.worldPos );
	float lambert  = max (dot ( lightDir , IN.normal), 0.0) * 0.9; 
	//specular
	vec3 viewDir = normalize ( cameraPos - IN.worldPos );
	vec3 halfDir = normalize ( lightDir + viewDir );
	float rFactor = max (dot ( halfDir , IN.normal ), 0.0);
	float sFactor = pow ( rFactor , 64.0 );

	//simple attenuation
    float dist = length(lightPos - IN.worldPos);
	float attenuation = 1.0 - clamp ( (dist* dist)/ (lightRadius*lightRadius) , 0.0 , 1.0);
	
	vec4 albedo = IN.colour;
	if(hasTexture) {
	 albedo *= texture(mainTex, IN.texCoord);
	}

	albedo.rgb = pow(albedo.rgb, vec3(2.2f));
	
	fragColor.rgb = albedo.rgb * 0.05f; //ambient
	
	fragColor.rgb += albedo.rgb * lightColour.rgb * lambert * shadow * attenuation; //diffuse light
	
	fragColor.rgb += lightColour.rgb * sFactor * shadow * attenuation; //specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0/2.2f));
	
	fragColor.a = albedo.a;
}