#include <duck/model/components/registerComponents.hpp>

#include <duck/model/components/cameraParameters.hpp>
#include <duck/model/components/mesh.hpp>
#include <duck/model/components/position.hpp>
#include <duck/model/components/rotation.hpp>
#include <duck/model/components/scale.hpp>
#include <duck/model/components/toUpdate.hpp>
#include <duck/model/components/curveControlPoints.hpp>


void RegisterAllComponents(Coordinator& coordinator)
{
    coordinator.RegisterComponent<CameraParameters>();
    coordinator.RegisterComponent<Mesh>();
    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Rotation>();
    coordinator.RegisterComponent<Scale>();
    coordinator.RegisterComponent<ToUpdate>();
    coordinator.RegisterComponent<CurveControlPoints>();
}
