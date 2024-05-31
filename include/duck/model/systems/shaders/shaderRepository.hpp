#pragma once

#include "stdShader.hpp"
#include "cubicBezierShader.hpp"
#include "cubeShader.hpp"
#include "waterShader.hpp"


class ShaderRepository {
public:
    inline const StdShader& GetStdShader() const
        { return stdShader; }

    inline const CubicBezierShader& GetBezierShader() const
        { return bezierShader; }

    inline const CubeShader& GetCubeShader() const
        { return cubeShader; }

    inline const WaterShader& GetWaterShader() const
        { return waterShader; }

private:
    StdShader stdShader;
    CubicBezierShader bezierShader;
    CubeShader cubeShader;
    WaterShader waterShader;
};
