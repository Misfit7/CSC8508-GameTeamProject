#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;
uniform sampler2D   bumpTex;
uniform sampler2D   specTex; 

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
	vec3 tangent;
    vec3 binormal;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	float shadow = 1.0; // New !
	
	if( IN . shadowProj . w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}
	
	mat3 TBN = mat3 ( normalize ( IN.tangent ) ,
               normalize ( IN.binormal ) ,
               normalize ( IN.normal ));
	
	vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	vec3  incident = normalize ( lightPos - IN.worldPos );
	float lambert  = max (0.0 , dot ( incident , bumpNormal )) * 0.9; 
	
	vec3 viewDir = normalize ( cameraPos - IN . worldPos );
	vec3 halfDir = normalize ( incident + viewDir );
	
	float spec = texture(specTex, IN.texCoord).r; 

	float rFactor = max (0.0 , dot ( halfDir , bumpNormal ));
	float sFactor = pow ( rFactor , spec );
	
	vec4 albedo = IN.colour;
	
	if(hasTexture) {
	 albedo *= texture(mainTex, IN.texCoord);
	}
	
	albedo.rgb = pow(albedo.rgb, vec3(2.2));
	
	fragColor.rgb = albedo.rgb * 0.05f; //ambient
	
	fragColor.rgb += albedo.rgb * lightColour.rgb * lambert * shadow; //diffuse light
	
	fragColor.rgb += lightColour.rgb * sFactor * shadow; //specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	fragColor.a = albedo.a;
}
