#include <duck/model/systems/waterSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/mesh.hpp>
#include <duck/model/components/waterPlane.hpp>
#include <duck/model/components/scale.hpp>

#include <duck/model/systems/cameraSystem.hpp>


void WaterSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<WaterSystem>();

    coordinator.RegisterRequiredComponent<WaterSystem, Mesh>();
    coordinator.RegisterRequiredComponent<WaterSystem, Scale>();
    coordinator.RegisterRequiredComponent<WaterSystem, WaterPlane>();
}


void WaterSystem::CreateWater(Entity entity, const WaterPlane &waterPlane) const
{
    Mesh mesh;
    mesh.Update(
        GenerateVertices(),
        GenerateIndices()
    );

    Scale scale(waterPlane.edge);

    coordinator->AddComponent<Mesh>(entity, mesh);
    coordinator->AddComponent<Scale>(entity, scale);
    coordinator->AddComponent<WaterPlane>(entity, waterPlane);
}


void WaterSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& shader = shaderRepo->GetWaterShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();
    Position cameraPos = cameraSystem->GetPosition();

    shader.Use();

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);

        shader.SetModelMat(scale.ScaleMatrix());
        shader.SetViewProjMat(cameraMtx);
        shader.SetCameraPosition(cameraPos.vec);
        mesh.Use();

        glDrawElements(GL_TRIANGLES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}


std::vector<float> WaterSystem::GenerateVertices() const
{
    return {
        // Vertex 0
        1.f, 0.f, 1.f,

        // Vertex 1
        1.f, 0.f, -1.f,

        // Vertex 2
        -1.f, 0.f, -1.f,

        // Vertex 3
        -1.f, 0.f, 1.f
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
