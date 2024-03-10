#pragma once

#include "stdShader.hpp"
#include "cubicBezierShader.hpp"


class ShaderRepository {
public:
    inline const StdShader& GetStdShader() const
        { return stdShader; }

    inline const CubicBezierShader& GetBezierShader() const
        { return bezierShader; }

private:
    StdShader stdShader;
    CubicBezierShader bezierShader;
};
