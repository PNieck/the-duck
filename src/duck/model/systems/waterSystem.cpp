#include <duck/model/systems/waterSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/mesh.hpp>
#include <duck/model/components/waterPlane.hpp>

#include <duck/model/systems/cameraSystem.hpp>


void WaterSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<WaterSystem>();

    coordinator.RegisterRequiredComponent<WaterSystem, Mesh>();
    coordinator.RegisterRequiredComponent<WaterSystem, WaterPlane>();
}


void WaterSystem::CreateWater(Entity entity, const WaterPlane &waterPlane) const
{
    Mesh mesh;
    mesh.Update(
        GenerateVertices(waterPlane),
        GenerateIndices()
    );

    coordinator->AddComponent<Mesh>(entity, mesh);
    coordinator->AddComponent<WaterPlane>(entity, waterPlane);
}


void WaterSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);

        shader.SetMVP(cameraMtx);
        mesh.Use();

        glDrawElements(GL_TRIANGLES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}


std::vector<float> WaterSystem::GenerateVertices(const WaterPlane& plane) const
{
    return {
        // Vertex 0
        plane.edge, 0.f, plane.edge,

        // Vertex 1
        plane.edge, 0.f, -plane.edge,

        // Vertex 2
        -plane.edge, 0.f, -plane.edge,

        // Vertex 3
        -plane.edge, 0.f, plane.edge
    };
}


std::vector<uint32_t> WaterSystem::GenerateIndices() const
{
    return {
        0, 1, 2,
        0, 2, 3,
        0, 2, 1,
        0, 3, 2
    };
}
