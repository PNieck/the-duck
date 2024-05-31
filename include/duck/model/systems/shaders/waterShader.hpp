#pragma once

#include "shader.hpp"


class WaterShader: private Shader {
public:
    WaterShader():
        Shader(
            "../../shaders/waterShader.vert",
            "../../shaders/waterShader.frag"
        ) {}

    inline void SetModelMat(const alg::Mat4x4& matrix) const
        { setMatrix4("modelMat", matrix); }

    inline void SetViewProjMat(const alg::Mat4x4& matrix) const
        { setMatrix4("viewProjMat", matrix); }

    inline void SetCameraPosition(const alg::Vec3& pos) const
        { setVec3("camPos", pos); }

    inline void Use() const
        { use(); }
};
