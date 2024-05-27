#version 410 core

in vec3 worldPos;

uniform samplerCube cubemap;

out vec4 FragColor;


void main()
{
    FragColor = texture(cubemap, worldPos);
    //FragColor = vec4(normalize(worldPos), 1.f);
}
