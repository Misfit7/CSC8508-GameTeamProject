#version 400

uniform mat4 modelMatrix 	  = mat4(1.0f);
uniform mat4 viewMatrix 	  = mat4(1.0f);
uniform mat4 projMatrix 	  = mat4(1.0f);

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in ivec4 jointIndices;

uniform mat4 joints[128];

void main(void){
vec4 localPos = vec4(position, 1.0f);
  vec4 skelPos = vec4(0,0,0,0);
 
  for(int i = 0; i < 4; ++i){
  int jointIndex = jointIndices[i];
  float jointWeight = jointWeights[i];
 
  skelPos += joints[jointIndex] * localPos * jointWeight;
}

  vec4 worldPos = (modelMatrix * vec4(skelPos.xyz, 1.0));
  gl_Position = (projMatrix * viewMatrix) * worldPos;
}
