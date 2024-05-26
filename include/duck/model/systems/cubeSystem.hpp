#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"


class CubeSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    void CreateCube(const Entity entity, float edge);

    void Render() const;

private:
    ShaderRepository* shaderRepo;

    std::vector<float> GenerateVertices(Entity entity) const;
    std::vector<uint32_t> GenerateIndices(Entity entity) const;
};
