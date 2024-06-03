#pragma once

#include <ecs/system.hpp>

#include <random>
#include <memory>

#include "pointsSystem.hpp"
#include "c2CurveSystem.hpp"


class FollowingCurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateCurve();

    void Update(Entity duck);

private:
    std::uniform_real_distribution<float> dist;
    std::mt19937 gen;

    float t;
    Entity curve;

    std::shared_ptr<PointsSystem> pointsSys;
    std::shared_ptr<C2CurveSystem> curveSys;
};
