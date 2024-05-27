#pragma once

#include <ecs/system.hpp>

#include "../components/waterPlane.hpp"

#include "shaders/shaderRepository.hpp"


class WaterSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    void CreateWater(Entity entity, const WaterPlane& waterPlane) const;

    void Render() const;

private:
    ShaderRepository* shaderRepo;

    std::vector<float> GenerateVertices(const WaterPlane& plane) const;
    std::vector<uint32_t> GenerateIndices() const;
};
