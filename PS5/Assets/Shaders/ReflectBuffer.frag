#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;
uniform sampler2D   bumpTex;
uniform samplerCube cubeTex;
uniform int emissive; 
uniform bool hasTexture;

uniform vec3 cameraPos;

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

out vec4 fragColour[5];

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
	
	vec4 albedo = IN.colour;
	if(hasTexture) {
	 albedo *= texture(mainTex, IN.texCoord);
	}
	
	vec3 viewDir = normalize(cameraPos - IN.worldPos);

    vec3 reflectDir = reflect(-viewDir, normalize (IN.normal));
    vec4 reflectTex = texture(cubeTex, reflectDir);
    
    albedo = reflectTex + (albedo * 0.25f); 
	
    fragColour [0] = albedo ;
    fragColour [1] = vec4 (bumpNormal.xyz * 0.5 + 0.5 ,1.0);
    fragColour [2] = vec4(0.2,0.2,0.2,1.0);
    fragColour [3] = vec4(emissive,1.0,1.0,1.0);
    fragColour [4] = vec4(shadow, 1.0, 1.0, 1.0);
}
