#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/meshWithTex.hpp"

#include <string>


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

    MeshWithTex GetMesh(const std::string& path) const;
};
