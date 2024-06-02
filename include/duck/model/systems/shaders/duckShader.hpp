#pragma once

#include "shader.hpp"


class DuckShader: private Shader {
public:
    DuckShader():
        Shader(
            "../../shaders/duckShader.vert",
            "../../shaders/duckShader.frag"
        ) {}

    inline void SetModelMat(const alg::Mat4x4& mat) const
        { setMatrix4("modelMat", mat); }

    inline void SetViewMat(const alg::Mat4x4& mat) const
        { setMatrix4("viewMat", mat); }

    inline void SetInvViewMat(const alg::Mat4x4& mat) const
        { setMatrix4("invViewMat", mat); }

    inline void SetProjMat(const alg::Mat4x4& mat) const
        { setMatrix4("projMat", mat); }

    inline void SetDuckTexUnit(int unit) const
        { setInt("duckTex", unit); }

    inline void Use() const
        { use(); }
};
