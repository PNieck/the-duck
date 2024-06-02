#include <duck/model/systems/cubeSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/mesh.hpp>
#include <duck/model/components/cubeParameters.hpp>
#include <duck/model/components/cubeMap.hpp>

#include <duck/model/systems/cameraSystem.hpp>
#include <duck/model/systems/pointsSystem.hpp>


void CubeSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<CubeSystem>();

    coordinator.RegisterRequiredComponent<CubeSystem, Mesh>();
    coordinator.RegisterRequiredComponent<CubeSystem, CubeParameters>();
    coordinator.RegisterRequiredComponent<CubeSystem, CubeMap>();
}


void CubeSystem::CreateCube(const Entity entity, float edge)
{
    CubeParameters params;
    params.edge = edge;

    coordinator->AddComponent<CubeParameters>(entity, params);

    auto const& vertices = GenerateVertices(entity);

    for (int i=0; i < vertices.size(); i += 3) {
        coordinator->GetSystem<PointsSystem>()->CreatePoint(Position(vertices[i], vertices[i+1], vertices[i+2]));
    }

    Mesh mesh;
    mesh.Update(
        vertices,
        GenerateIndices(entity)
    );

    coordinator->AddComponent<Mesh>(entity, mesh);

    CubeMap cubeMap(
        "../../resources/textures/MountainPath/posx.jpg",
        "../../resources/textures/MountainPath/negx.jpg",

        "../../resources/textures/MountainPath/posy.jpg",
        "../../resources/textures/MountainPath/negy.jpg",

        "../../resources/textures/MountainPath/posz.jpg",
        "../../resources/textures/MountainPath/negz.jpg"
    );

    coordinator->AddComponent<CubeMap>(entity, cubeMap);
}


void CubeSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& shader = shaderRepo->GetCubeShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& cubeMap = coordinator->GetComponent<CubeMap>(entity);

        shader.SetMVP(cameraMtx);
        shader.SetTexSampler(0);
        cubeMap.Use();
        mesh.Use();

        glDrawElements(GL_TRIANGLES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}


std::vector<float> CubeSystem::GenerateVertices(Entity entity) const
{
    auto const& cubeParams = coordinator->GetComponent<CubeParameters>(entity);
    //auto const& cubePos = coordinator->GetComponent<Position>(entity);

    return {
        // Vertex 1
        cubeParams.edge,
        cubeParams.edge,
        cubeParams.edge,

        // Vertex 2
        cubeParams.edge,
        -cubeParams.edge,
        cubeParams.edge,

        // Vertex 3
        cubeParams.edge,
        -cubeParams.edge,
        -cubeParams.edge,

        // Vertex 4
        cubeParams.edge,
        cubeParams.edge,
        -cubeParams.edge,

        // Vertex 5
        -cubeParams.edge,
        cubeParams.edge,
        cubeParams.edge,

        // Vertex 6
        -cubeParams.edge,
        -cubeParams.edge,
        cubeParams.edge,

        // Vertex 7
        -cubeParams.edge,
        -cubeParams.edge,
        -cubeParams.edge,

        // Vertex 8
        -cubeParams.edge,
        cubeParams.edge,
        -cubeParams.edge
    };
}


std::vector<uint32_t> CubeSystem::GenerateIndices(Entity entity) const
{
	return {
		0, 2, 1,
        0, 3, 2,
        4, 5, 6,
        4, 6, 7,
        4, 1, 5,
        4, 0, 1,
        7, 6, 2,
        7, 2, 3,
        7, 0, 4,
        7, 3, 0,
        5, 1, 6,
        6, 1, 2
	};
}
