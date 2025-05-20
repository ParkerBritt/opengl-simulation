#version 430 core

in vec4 vWorldPos;
in vec3 vNormal;
out vec4 color;

float remap(float value, float inMin, float inMax, float outMin, float outMax)
{
    return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

vec3 remap(vec3 value, vec3 inMin, vec3 inMax, vec3 outMin, vec3 outMax)
{
    return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

void main()
{
    // color = vWorldPos;
    vec3 lightDir = normalize(vec3(1.0,1.0,1.0));
    float brightness = remap(dot(vNormal, lightDir), -1, 1, 0.5, 1);
    color = vec4(vec3(brightness), 1.0f);
    // color = vec4(1.0f, 0.5, 0.0f, 1.0f);
}
