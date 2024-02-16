#version 330 core
uniform sampler2D sceneTex;
uniform sampler2D skyboxTex;
uniform sampler2D depthTex;
uniform int isVertical;

in Vertex{
    vec2 texCoord;
} IN;
out vec4 fragColor;

const float scaleFactors[7] = float[](0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006);

void main(void) {
	vec4 depth = texture (depthTex, IN.texCoord);
    fragColor = vec4(0, 0, 0, 1);

    vec2 delta = vec2(0, 0);

    if (isVertical == 1) {
        delta = dFdy(IN.texCoord);
    }
    else {
        delta = dFdx(IN.texCoord);
    }

    for (int i = 0; i < 7; i++) {
        vec2 offset = delta * (i - 3);
        vec2 sampleCoord = IN.texCoord.xy + offset;
		
		if (depth.r < 1) {
			vec4 tmp1 = texture2D(sceneTex, sampleCoord);
			fragColor += tmp1 * scaleFactors[i];
		}
		else{
			vec4 tmp2 = texture2D(skyboxTex, sampleCoord);
			fragColor += tmp2 * scaleFactors[i];
		}
       
        
    }
}

