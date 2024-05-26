#version 410 core


uniform mat4 view;
uniform mat4 proj;
layout (location = 0) in vec3 pos;


out vec3 nearPoint;
out vec3 farPoint;


vec3 UnprojectPoint(float x, float y, float z) {
    mat4 inv = inverse(proj * view);
    vec4 unprojectedPoint =  inv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}


void main() {
    nearPoint = UnprojectPoint(pos.x, pos.y, -1.0f); // unprojecting on the near plane
    farPoint = UnprojectPoint(pos.x, pos.y, 1.0f);  // unprojecting on the far plane

    gl_Position = vec4(pos, 1.0);
}
