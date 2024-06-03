#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/meshWithTex.hpp"

#include <string>
#include <random>


class DuckSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    void CreateDuck(Entity entity);

    void Update() const;

    void Render() const;
private:
    ShaderRepository* shaderRepo;

    Entity curve;

    std::uniform_real_distribution<float> dist;
    std::mt19937 gen;

    MeshWithTex GetMesh(const std::string& path) const;
};
