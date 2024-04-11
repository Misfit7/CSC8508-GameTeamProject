#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D blurbloomTex;
uniform int count;

out vec4 fragColour;

in Vertex{
    vec2 texCoord;
} IN;


void main(void) {
	if(count==0){
		vec3 hdrColor = texture(diffuseTex, IN.texCoord).rgb;
		vec3 bloomColor = texture(blurbloomTex, IN.texCoord).rgb;
		hdrColor+=bloomColor;
		vec3 result = vec3(1.0) - exp(hdrColor * -2.0);
		fragColour = vec4(result, 1.0);
	}
    else if(count==1){
		fragColour = texture(diffuseTex, IN.texCoord);
	}
}