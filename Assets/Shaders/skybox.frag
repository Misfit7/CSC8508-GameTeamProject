#version 330 core

uniform samplerCube cubeTex;
uniform samplerCube nightCubeTex;

uniform float gradientFactor;
//uniform float transitionFactor;

in Vertex {
    vec3 viewDir;
} IN;

out vec4 fragColour;

void main(void) {
    //vec4 skyColor = texture(cubeTex, normalize(IN.viewDir));
    //vec4 nightColor = texture(nightCubeTex, normalize(IN.viewDir)); 
	//
    //vec4 sunColor = vec4(1.0, 0.5, 0.0, 1.0);
	//
    //vec4 currentColor;
    //vec4 timeBasedColor = mix(nightColor, skyColor, gradientFactor);
	//
    //if(gradientFactor > 0.0 && gradientFactor < 1.0) {
    //    float sunIntensity = pow(1.0 - abs(gradientFactor - 0.5) * 2.0, 2.0);
    //    timeBasedColor = mix(timeBasedColor, sunColor, sunIntensity * 0.1);
    //}
	//

	vec4 samp = texture(cubeTex,normalize(IN.viewDir));
	fragColour = pow(samp, vec4(2.2f));
	fragColour.a = gradientFactor;
}