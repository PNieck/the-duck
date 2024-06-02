#pragma once

#include <ecs/system.hpp>

#include <random>


class RainSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Update(Entity water);

private:
    std::uniform_real_distribution<float> dist;
    std::mt19937 gen;
};
