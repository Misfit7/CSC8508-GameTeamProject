#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2D skyboxTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D emissiveLight;

in Vertex {
vec2 texCoord ;
} IN ;

 out vec4 fragColour ;

 void main ( void ) {
 vec3 diffuse = texture ( diffuseTex , IN.texCoord ).xyz ;
 vec3 light = texture ( diffuseLight , IN.texCoord ).xyz ;
 vec3 specular = texture ( specularLight , IN.texCoord ).xyz ;
 vec4 depth = texture (depthTex, IN.texCoord);
 vec4 cube = texture (skyboxTex, IN.texCoord);
 
 if (depth.r < 1) {
  if(texture(emissiveLight, IN.texCoord.xy).r == 1.0){
   fragColour.xyz = diffuse * 1.0;
  }
  else{
   fragColour.xyz = diffuse * light; // lambert
   fragColour.xyz += specular; // Specular
   fragColour.xyz += diffuse * 0.01; // ambient
  }
  fragColour.a = 1.0;
}
 else{
  fragColour = cube;
 }
}
