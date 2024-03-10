#include <duck/model/systems/toUpdateSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/toUpdate.hpp>


void ToUpdateSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<ToUpdateSystem>();

    coordinator.RegisterRequiredComponent<ToUpdateSystem, ToUpdate>();
}
