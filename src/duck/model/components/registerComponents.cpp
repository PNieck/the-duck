#include <duck/model/components/registerComponents.hpp>

#include <duck/model/components/cameraParameters.hpp>
#include <duck/model/components/mesh.hpp>
#include <duck/model/components/position.hpp>
#include <duck/model/components/rotation.hpp>
#include <duck/model/components/scale.hpp>
#include <duck/model/components/toUpdate.hpp>
#include <duck/model/components/curveControlPoints.hpp>
#include <duck/model/components/cubeParameters.hpp>
#include <duck/model/components/cubeMap.hpp>
#include <duck/model/components/waterPlane.hpp>
#include <duck/model/components/texture.hpp>
#include <duck/model/components/meshWithTex.hpp>


void RegisterAllComponents(Coordinator& coordinator)
{
    coordinator.RegisterComponent<CameraParameters>();
    coordinator.RegisterComponent<Mesh>();
    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Rotation>();
    coordinator.RegisterComponent<Scale>();
    coordinator.RegisterComponent<ToUpdate>();
    coordinator.RegisterComponent<CurveControlPoints>();
    coordinator.RegisterComponent<CubeParameters>();
    coordinator.RegisterComponent<CubeMap>();
    coordinator.RegisterComponent<WaterPlane>();
    coordinator.RegisterComponent<Texture>();
    coordinator.RegisterComponent<MeshWithTex>();
}
