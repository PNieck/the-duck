#version 410 core


in vec3 worldPos;
in vec4 pos;
in vec3 normalVec;
in vec3 viewVec;
in vec2 tex;


uniform sampler2D duckTex;

out vec4 FragColor;

void main()
{
    FragColor = texture(duckTex, tex);
}
