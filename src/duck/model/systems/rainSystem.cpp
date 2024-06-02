#include <duck/model/systems/rainSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/systems/waterSystem.hpp>


void RainSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<RainSystem>();
}


void RainSystem::Update(Entity water)
{
    if (dist(gen) < 0.03f) {
        auto waterSys = coordinator->GetSystem<WaterSystem>();
        waterSys->Disturb(water, dist(gen), dist(gen));
    }
}
