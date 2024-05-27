#version 410 core

layout (location = 0) in vec3 aPos;

out vec3 worldPos;

uniform mat4 MVP;

void main()
{
    worldPos = aPos;
    gl_Position = MVP * vec4(aPos, 1.0);
}
