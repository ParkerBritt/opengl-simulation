#version 410 core

in vec4 position;
uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 offsets[300];
void main()
{
   vec3 offset = offsets[gl_InstanceID];
   gl_Position = uProj * uView * vec4(position.xyz+offset, position.w);
}

