#version 410 core


layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTex;


uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 invViewMat;
uniform mat4 projMat;


out vec3 worldPos;
out vec4 pos;
out vec3 normalVec;
out vec3 viewVec;
out vec2 tex;


void main()
{
   worldPos = (modelMat * vec4(inPos, 1.0f)).xyz;
   pos = viewMat * vec4(worldPos, 1.0f);
   pos = projMat * pos;

   normalVec = (modelMat * vec4(inNormal, 0.f)).xyz;
   normalVec = normalize(normalVec);

   vec3 camPos = (invViewMat * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
   viewVec = camPos - worldPos;

   tex = inTex;

   gl_Position = pos;
}
