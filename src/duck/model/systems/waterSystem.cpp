#include <duck/model/systems/waterSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/mesh.hpp>
#include <duck/model/components/waterPlane.hpp>
#include <duck/model/components/scale.hpp>
#include <duck/model/components/texture.hpp>
#include <duck/model/components/cubeMap.hpp>

#include <duck/model/systems/cameraSystem.hpp>

#include <cmath>


void WaterSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<WaterSystem>();

    coordinator.RegisterRequiredComponent<WaterSystem, Mesh>();
    coordinator.RegisterRequiredComponent<WaterSystem, Scale>();
    coordinator.RegisterRequiredComponent<WaterSystem, Texture>();
    coordinator.RegisterRequiredComponent<WaterSystem, WaterPlane>();
}


void WaterSystem::CreateWater(Entity entity, float edge) const
{
    Mesh mesh;
    mesh.Update(
        GenerateVertices(),
        GenerateIndices()
    );

    Scale scale(edge);
    Texture texture;

    coordinator->AddComponent<Mesh>(entity, mesh);
    coordinator->AddComponent<Scale>(entity, scale);
    coordinator->AddComponent<Texture>(entity, texture);
    coordinator->AddComponent<WaterPlane>(entity, InitWaterPlane(edge));
}


void WaterSystem::Update() const
{
    for (auto entity: entities) {
        UpdateWaterPlane(entity);
        UpdateNormalMap(entity);
    }
}


void WaterSystem::Render(Entity cubeMap) const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& envMap = coordinator->GetComponent<CubeMap>(cubeMap);
    
    auto const& shader = shaderRepo->GetWaterShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();
    Position cameraPos = cameraSystem->GetPosition();

    shader.Use();

    glActiveTexture(GL_TEXTURE0);
    envMap.Use();
    shader.SetEnvCubeTextureUnit(0);

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& texture = coordinator->GetComponent<Texture>(entity);

        shader.SetModelMat(scale.ScaleMatrix());
        shader.SetViewProjMat(cameraMtx);
        shader.SetCameraPosition(cameraPos.vec);
        mesh.Use();

        glActiveTexture(GL_TEXTURE1);
        texture.Use();
        shader.SetNormalMapTextureUnit(1);

        glDrawElements(GL_TRIANGLES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}


void WaterSystem::Disturb(Entity water, float u, float v) const
{
    coordinator->EditComponent<WaterPlane>(water,
        [u, v](WaterPlane& plane) {
            int row = int(u * heightMapResolution);
            int col = int(v * heightMapResolution);
            plane.actHeightMap[row][col] += 0.25f;
        } 
    );
}


void WaterSystem::UpdateWaterPlane(Entity entity) const
{
    coordinator->EditComponent<WaterPlane>(entity,
        [this](WaterPlane& plane) {
            plane.actHeightMap.swap(plane.prevHeightMap);

            const float deltaT = 1.f / float(heightMapResolution);
            const float h = 2.f / (float(heightMapResolution - 1));
            const float c = 1.f;
            const float A = (c*c * deltaT * deltaT) / (h * h);
            const float B = 2.f - 4.f * A;

            for (int row=1; row < heightMapResolution - 1; row++) {
                for (int col=1; col < heightMapResolution - 1; col++) {
                    float d = GetDampingFactor(plane, row, col);

                    plane.actHeightMap[row][col] = d * (A*(
                        plane.prevHeightMap[row+1][col] +
                        plane.prevHeightMap[row-1][col] +
                        plane.prevHeightMap[row][col-1] +
                        plane.prevHeightMap[row][col+1]) +
                        B*plane.prevHeightMap[row][col] -
                        plane.actHeightMap[row][col]);
                }
            }
        }
    );
}


void WaterSystem::UpdateNormalMap(Entity entity) const
{
    coordinator->EditComponent<Texture>(entity,
        [this, entity] (Texture& texture) {
            auto const& waterPlane = coordinator->GetComponent<WaterPlane>(entity);

            std::vector<GLubyte> normalMap;
            normalMap.reserve(heightMapResolution * heightMapResolution * 3);

            for (int row=1; row < heightMapResolution-1; row++) {
                for (int col=1; col < heightMapResolution-1; col++) {
                    alg::Vec3 height = GetHeightMapElemPos(waterPlane, row, col);
                    alg::Vec3 heightZ = GetHeightMapElemPos(waterPlane, row+1, col);
                    alg::Vec3 heightY = GetHeightMapElemPos(waterPlane, row, col+1);

                    alg::Vec3 normal = alg::Cross(heightZ - height, heightY - height).Normalize();

                    normalMap.push_back((GLubyte)((normal.X() + 1.f)/2.f * 255));
                    normalMap.push_back((GLubyte)((normal.Y() + 1.f)/2.f * 255));
                    normalMap.push_back((GLubyte)((normal.Z() + 1.f)/2.f * 255));
                }
            }

            texture.Use();
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            texture.Update(heightMapResolution-2, heightMapResolution-2, GL_RGB, normalMap.data());
        }
    );
}


WaterPlane WaterSystem::InitWaterPlane(float edge) const
{
    WaterPlane result {
        .edge = edge,
        .actHeightMap = std::move(std::vector<std::vector<float>>(heightMapResolution)),
        .prevHeightMap = std::move(std::vector<std::vector<float>>(heightMapResolution)),
    };

    for (int i=0; i < heightMapResolution; i++) {
        result.actHeightMap[i] = std::move(std::vector<float>(heightMapResolution, 0.f));
        result.prevHeightMap[i] = std::move(std::vector<float>(heightMapResolution, 0.f));
    }

    return result;
}


float WaterSystem::GetDampingFactor(const WaterPlane &plane, int row, int col) const
{
    alg::Vec3 pos = GetHeightMapElemPos(plane, row, col);

    float wallPos = float(plane.edge) / 2.f;

    // distance to the nearest floor
    float l = std::min(
        std::min(std::abs(wallPos - pos.X()), std::abs(-wallPos - pos.X())),
        std::min(std::abs(wallPos - pos.Z()), std::abs(-wallPos - pos.Z()))
    );

    return 0.95f * std::min(1.f, l/0.2f);
}


alg::Vec3 WaterSystem::GetHeightMapElemPos(const WaterPlane &plane, int row, int col) const
{
    float offsetElem = float(plane.edge) / float(heightMapResolution);
    alg::Vec3 offsetX(offsetElem, 0.f, 0.f);
    alg::Vec3 offsetZ(0.f, 0.f, offsetElem);

    alg::Vec3 point00 = -float(heightMapResolution/2) * offsetX - float(heightMapResolution/2) * offsetZ;
    alg::Vec3 actHeight(0.f, plane.actHeightMap[row][col], 0.f);

    return point00 + float(row) * offsetZ + float(col) * offsetX + actHeight;
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
