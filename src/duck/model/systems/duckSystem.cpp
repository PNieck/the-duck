#include <duck/model/systems/duckSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/position.hpp>
#include <duck/model/components/rotation.hpp>
#include <duck/model/components/scale.hpp>
#include <duck/model/components/texture.hpp>

#include <duck/model/systems/cameraSystem.hpp>
#include <duck/model/systems/waterSystem.hpp>

#include <fstream>
#include <vector>


void DuckSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<DuckSystem>();

    coordinator.RegisterRequiredComponent<DuckSystem, Position>();
    coordinator.RegisterRequiredComponent<DuckSystem, Rotation>();
    coordinator.RegisterRequiredComponent<DuckSystem, Scale>();
    coordinator.RegisterRequiredComponent<DuckSystem, MeshWithTex>();
    coordinator.RegisterRequiredComponent<DuckSystem, Texture>();
}


void DuckSystem::CreateDuck(Entity entity)
{
    coordinator->AddComponent(entity, Position(0.f));
    coordinator->AddComponent(entity, Scale(1.f/400.f, 1.f/400.f, 1.f/400.f));
    coordinator->AddComponent(entity, Rotation());

    MeshWithTex mesh = GetMesh("../../resources/models/duck.txt");
    coordinator->AddComponent(entity, mesh);

    Texture tex("../../resources/textures/ducktex.jpg");
    coordinator->AddComponent(entity, tex);
}


void DuckSystem::Update(Entity water, Entity duck) const
{
    auto const& pos = coordinator->GetComponent<Position>(duck);

    float u = pos.GetX() / 4.f + 0.5f;
    float v = pos.GetZ() / 4.f + 0.5f;
    coordinator->GetSystem<WaterSystem>()->Disturb(water, v, u);
}


void DuckSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& shader = shaderRepo->GetDuckShader();

    shader.Use();
    shader.SetProjMat(cameraSystem->PerspectiveMatrix());
    shader.SetViewMat(cameraSystem->ViewMatrix());
    shader.SetInvViewMat(cameraSystem->ViewMatrix().Inverse().value());

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<MeshWithTex>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);
        auto const& tex = coordinator->GetComponent<Texture>(entity);

        alg::Mat4x4 modelMtx = position.TranslationMatrix() * rotation.GetRotationMatrix() * scale.ScaleMatrix();
        shader.SetModelMat(modelMtx);
        
        glActiveTexture(GL_TEXTURE0);
        tex.Use();
        shader.SetDuckTexUnit(0);

        mesh.Use();
        glDrawElements(GL_TRIANGLES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
        
    }
}


MeshWithTex DuckSystem::GetMesh(const std::string& path) const
{
    std::ifstream input;
    // In general we shouldn't throw exceptions on end-of-file,
    // however, in case of this file format if we reach the end
    // of a file before we read all values, the file is
    // ill-formated and we would need to throw an exception anyway
    input.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
    input.open(path);

    int verticesCnt, indicesCnt;

    input >> verticesCnt;
    std::vector<float> vertices(verticesCnt * 8);

    for (int i = 0; i < verticesCnt * 8; i += 8) {
        input >> vertices[i] >> vertices[i+1] >> vertices[i+2];     // vertex position
        input >> vertices[i+3] >> vertices[i+4] >> vertices[i+5];   // vertex normals
        input >> vertices[i+6] >> vertices[i+7];                    // vertex texture coordinates
    }

    input >> indicesCnt;
    std::vector<uint32_t> indices(indicesCnt*3);

    for (int i = 0; i < indicesCnt * 3; i += 3) {
        input >> indices[i] >> indices[i+1] >> indices[i+2];
    }

    MeshWithTex mesh;
    mesh.Update(
        vertices,
        indices
    );

    return mesh;
}
