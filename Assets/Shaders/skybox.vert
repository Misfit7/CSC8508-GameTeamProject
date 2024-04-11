#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time;

layout(location = 0) in vec3 position;

out Vertex {
    vec3 viewDir;
} OUT;

mat4 rotate(float angle, vec3 axis) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main(void) {
    vec3 pos = position;
    mat4 invproj = inverse(projMatrix);
    pos.xy *= vec2(invproj[0][0], invproj[1][1]);
    pos.z = -1.0f;

    float angle = time * 1;
    mat4 rotationMatrix = rotate(angle, vec3(0, 1, 0));
    mat4 rotatedView = rotationMatrix * viewMatrix;

    OUT.viewDir = transpose(mat3(rotatedView)) * normalize(pos);
    gl_Position		= vec4(position, 1.0);
}

