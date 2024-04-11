#version 400 core

out vec4 fragColor;
layout (location = 5) out vec4 BrightColor;

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

void main(void)
{
	float shadow = 1.0; // New !
	
	if( IN.shadowProj.w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}

	//diffuse
	vec3  lightDir = normalize ( lightPos - IN.worldPos );
	float lambert  = max (dot ( lightDir , IN.normal), 0.0); 
	//specular
	vec3 viewDir = normalize ( cameraPos - IN.worldPos );
	vec3 halfDir = normalize ( lightDir + viewDir );
	float rFactor = max (dot ( halfDir , IN.normal ), 0.0);
	float sFactor = pow ( rFactor , 64.0 );

	//simple attenuation
    float dist = length(lightPos - IN.worldPos);
	float attenuation = 1.0 - clamp ( (dist* dist)/ (lightRadius*lightRadius) , 0.0 , 1.0);
	
	vec4 color = IN.colour;
	if(hasTexture) {
	 color *= texture(mainTex, IN.texCoord);
	}

	vec3 ambient =  color.rgb * 0.025f; //ambient
	
	vec3 lighting = vec3(0.0);
	
	lighting += lightColour.rgb * lambert ; //diffuse light
	
	lighting += lightColour.rgb * sFactor ; //specular light
	
	lighting*= (shadow * attenuation);

	color.rgb *=(ambient+lighting);
	
	vec3 result = vec3(1.0) - exp(-color.rgb * 1.05 ); //hdr
	
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722)); //bloom
	if(brightness > 1.0)
		BrightColor = vec4(color.rgb, color.a);
	else
		BrightColor = vec4(result, color.a);
	
	fragColor.rgb = pow(result, vec3(1.0/2.2f)); //gamma
		
	fragColor.a = color.a;
}