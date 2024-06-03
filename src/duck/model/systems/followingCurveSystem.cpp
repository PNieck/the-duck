#include <duck/model/systems/followingCurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/rotation.hpp>


void FollowingCurveSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<FollowingCurveSystem>();
}


void FollowingCurveSystem::Init()
{
    pointsSys = coordinator->GetSystem<PointsSystem>();
    curveSys = coordinator->GetSystem<C2CurveSystem>();

    t = 0.f;
}


Entity FollowingCurveSystem::CreateCurve()
{
    Entity cp1 = pointsSys->CreatePoint(Position(dist(gen) * 2.f, 0.f, dist(gen) * 2.f));
    Entity cp2 = pointsSys->CreatePoint(Position(dist(gen) * 2.f, 0.f, dist(gen) * 2.f));
    Entity cp3 = pointsSys->CreatePoint(Position(dist(gen) * 2.f, 0.f, dist(gen) * 2.f));
    Entity cp4 = pointsSys->CreatePoint(Position(dist(gen) * 2.f, 0.f, dist(gen) * 2.f));

    curve = curveSys->CreateC2Curve({cp1, cp2, cp3, cp4});

    return curve;
}


void FollowingCurveSystem::Update(Entity duck)
{
    static const float deltaT = 1.f/256.f;

    Position actPos = coordinator->GetComponent<Position>(duck);

    t += deltaT;

    if (t > 1.0f) {
        auto const& cps = coordinator->GetComponent<CurveControlPoints>(curve);
        Entity toDel = *cps.ControlPoints().begin();
        curveSys->DeleteControlPoint(curve, toDel);
        coordinator->DestroyEntity(toDel);

        Entity newCp = pointsSys->CreatePoint(Position(dist(gen) * 2.f, 0.f, dist(gen) * 2.f));
        curveSys->AddControlPoint(curve, newCp);

        t -= 1.f;
    }

    Position newPos = curveSys->CalculatePosition(curve, t);
    coordinator->SetComponent<Position>(duck, newPos);

    alg::Vec3 deltaPos = newPos.vec - actPos.vec;
    deltaPos = deltaPos.Normalize();

    coordinator->SetComponent<Rotation>(duck, Rotation(alg::Quat(alg::Vec3(-1.f, 0.f, 0.f), deltaPos)));
}
