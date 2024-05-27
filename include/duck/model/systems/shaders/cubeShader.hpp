#pragma once

#include "shader.hpp"


class CubeShader: private Shader {
public:
    CubeShader():
        Shader(
            "../../shaders/cubeShader.vert",
            "../../shaders/cubeShader.frag"
        ) {}

    inline void SetMVP(const alg::Mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    inline void SetTexSampler(int texId) const
        { setInt("cubemap", texId); }

    inline void Use() const
        { use(); }
};
