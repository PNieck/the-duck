#include <duck/model/systems/pointsSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/systems/cameraSystem.hpp>

#include <duck/model/components/position.hpp>


void PointsSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<PointsSystem>();
}


PointsSystem::PointsSystem()
{
    std::vector<float> vertices = {
        0.0f, 0.0f, 0.0f
    };

    std::vector<uint32_t> indices = { 0 };

    pointsMesh.Update(vertices, indices);
}


Entity PointsSystem::CreatePoint(const Position& pos)
{
    Entity newPoint = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(newPoint, pos);

    entities.insert(newPoint);

    return newPoint;
}


void PointsSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto const entity : entities) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        shader.SetMVP(cameraMtx * position.TranslationMatrix());
        
        pointsMesh.Use();
        glDrawElements(GL_POINTS, pointsMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}
