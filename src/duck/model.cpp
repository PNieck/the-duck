#include <duck/model.hpp>

#include <duck/utilities/line.hpp>
#include <duck/utilities/plane.hpp>
#include <duck/utilities/angle.hpp>

#include <duck/model/components/registerComponents.hpp>
#include <duck/model/components/cameraParameters.hpp>

#include <duck/model/systems/toUpdateSystem.hpp>
#include <duck/model/systems/curveControlPointsSystem.hpp>

#include <stdexcept>


Model::Model(int viewport_width, int viewport_height)
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<uint32_t>::max());

    RegisterAllComponents(coordinator);

    CameraSystem::RegisterSystem(coordinator);
    CurveControlPointsSystem::RegisterSystem(coordinator);
    ToUpdateSystem::RegisterSystem(coordinator);
    CubeSystem::RegisterSystem(coordinator);
    PointsSystem::RegisterSystem(coordinator);
    WaterSystem::RegisterSystem(coordinator);
    RainSystem::RegisterSystem(coordinator);

    cameraSys = coordinator.GetSystem<CameraSystem>();
    auto curveControlPointsSystem = coordinator.GetSystem<CurveControlPointsSystem>();
    cubeSys = coordinator.GetSystem<CubeSystem>();
    pointsSys = coordinator.GetSystem<PointsSystem>();
    waterSys = coordinator.GetSystem<WaterSystem>();
    rainSystem = coordinator.GetSystem<RainSystem>();

    CameraParameters params {
        .target = Position(0.0f),
        .viewportWidth = viewport_width,
        .viewportHeight = viewport_height,
        .fov = Angle::FromDegrees(45.f).ToRadians(),
        .near_plane = 0.1f,
        .far_plane = 100.0f,
    };

    cameraSys->Init(params, Position(0.0f, 0.0f, 10.0f));
    curveControlPointsSystem->Init();
    cubeSys->Init(&shadersRepo);
    pointsSys->Init(&shadersRepo);
    waterSys->Init(&shadersRepo);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    cube = coordinator.CreateEntity();
    cubeSys->CreateCube(cube, 2.f);

    water = coordinator.CreateEntity();
    waterSys->CreateWater( water, 2.f);
}


void Model::RenderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rainSystem->Update(water);
    waterSys->Update();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    waterSys->Render(cube);
    cubeSys->Render();
    //pointsSys->Render();
}


void Model::ChangeViewportSize(int width, int height)
{
    glViewport(0, 0, width, height);
    cameraSys->ChangeViewportSize(width, height);
}


Line Model::LineFromViewportCoordinates(float x, float y) const
{
    auto viewMtx = cameraSys->ViewMatrix();
    auto perspectiveMtx = cameraSys->PerspectiveMatrix();

    auto cameraInv = (perspectiveMtx * viewMtx).Inverse().value();

    alg::Vec4 nearV4 = cameraInv * alg::Vec4(x, y, 1.0f, 1.0f);
    alg::Vec4 farV4 = cameraInv * alg::Vec4(x, y, -1.0f, 1.0f);

    alg::Vec3 near = alg::Vec3(
        nearV4.X() / nearV4.W(),
        nearV4.Y() / nearV4.W(),
        nearV4.Z() / nearV4.W()
    );

    alg::Vec3 far = alg::Vec3(
        farV4.X() / farV4.W(),
        farV4.Y() / farV4.W(),
        farV4.Z() / farV4.W()
    );

    return Line::FromTwoPoints(near, far);
}
