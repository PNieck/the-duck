#pragma once

#include <ecs/system.hpp>

#include "../components/waterPlane.hpp"

#include "shaders/shaderRepository.hpp"


class WaterSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    void CreateWater(Entity entity, float edge) const;

    void Update() const;

    void Render(Entity cubeMap) const;

private:
    static constexpr int heightMapResolution = 256;

    ShaderRepository* shaderRepo;

    void UpdateWaterPlane(Entity entity) const;
    void UpdateNormalMap(Entity entity) const;

    WaterPlane InitWaterPlane(float edge) const;
    float GetDampingFactor(const WaterPlane& plane, int row, int col) const;
    alg::Vec3 GetHeightMapElemPos(const WaterPlane& plane, int row, int col) const;

    std::vector<float> GenerateVertices() const;
    std::vector<uint32_t> GenerateIndices() const;
};
