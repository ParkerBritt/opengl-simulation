#version 430 core

in vec4 position;
uniform mat4 uView;
uniform mat4 uProj;
layout(location = 1) in vec3 aNormal;

layout(std430, binding = 3) buffer InstancePositionBuffer {
    vec4 instancePositions[];
};

out vec4 vWorldPos;
out vec3 vNormal;

void main()
{
    vNormal = aNormal;
    vec4 offset = instancePositions[gl_InstanceID];
    vWorldPos = uProj * (vec4(position.xyz, position.w)+offset); 
    vec4 vViewPos = uProj * uView * (vec4(position.xyz, position.w)+offset);
    
    gl_Position = vViewPos;
}

