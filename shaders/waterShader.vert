#version 410 core

layout (location = 0) in vec3 aPos;

uniform mat4 modelMat;
uniform mat4 viewProjMat;

out vec3 localPos;
out vec3 worldPos;
out vec4 pos;

void main()
{
    localPos = aPos;
    worldPos =  (modelMat * vec4(aPos, 1.0f)).xyz;
    pos = viewProjMat * vec4(worldPos, 1.0f);
    gl_Position = pos;
}
