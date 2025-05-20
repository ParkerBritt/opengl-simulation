#version 430 core

in vec4 position;
uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 offsets[1000];

layout(std430, binding = 3) buffer InstancePositionBuffer {
    vec4 instancePositions[];
};

void main()
{
    vec4 offset = instancePositions[gl_InstanceID];
    
    gl_Position = uProj * uView * (vec4(position.xyz, position.w)+offset);
}

