#include <duck/model/systems/c2CurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <duck/model/components/mesh.hpp>
#include <duck/model/components/position.hpp>
#include <duck/model/components/curveControlPoints.hpp>
#include <duck/model/components/c2CurveParameters.hpp>
#include <duck/model/components/bSplinePolygonMesh.hpp>

#include <duck/model/systems/cameraSystem.hpp>
#include <duck/model/systems/toUpdateSystem.hpp>
#include <duck/model/systems/pointsSystem.hpp>
#include <duck/model/systems/curveControlPointsSystem.hpp>

#include <duck/utilities/setIntersection.hpp>

#include <algorithm>


void C2CurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<C2CurveSystem>();

    coordinator.RegisterRequiredComponent<C2CurveSystem, C2CurveParameters>();
    coordinator.RegisterRequiredComponent<C2CurveSystem, CurveControlPoints>();
    coordinator.RegisterRequiredComponent<C2CurveSystem, Mesh>();
}


Entity C2CurveSystem::CreateC2Curve(const std::vector<Entity>& entities)
{
    Entity curve = coordinator->GetSystem<CurveControlPointsSystem>()->CreateControlPoints(entities);
    auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(curve);

    C2CurveParameters params;

    Mesh mesh;
    mesh.Update(
        GenerateCurveMeshVertices(controlPoints),
        GenerateCurveMeshIndices(controlPoints)
    );

    coordinator->AddComponent<C2CurveParameters>(curve, params);
    coordinator->AddComponent<Mesh>(curve, mesh);

    return curve;
}


void C2CurveSystem::ShowBSplinePolygon(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.drawBSplinePolygon = true;
        }
    );

    auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(entity);
    BSplinePolygonMesh polygonMesh;

    polygonMesh.Update(
        GenerateBSplinePolygonVertices(controlPoints),
        GenerateBSplinePolygonIndices(controlPoints)
    );

    coordinator->AddComponent<BSplinePolygonMesh>(entity, polygonMesh);
}


void C2CurveSystem::HideBSplinePolygon(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.drawBSplinePolygon = false;
        }
    );

    coordinator->DeleteComponent<BSplinePolygonMesh>(entity);
}


void C2CurveSystem::ShowBezierPolygon(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.drawBezierPolygon = true;
        }
    );
}


void C2CurveSystem::HideBezierPolygon(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.drawBezierPolygon = false;
        }
    );
}


void C2CurveSystem::ShowBezierControlPoints(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.showBezierControlPoints = true;
        }
    );

    auto bezierControlPoints = CreateBezierControlPoints(
        coordinator->GetComponent<CurveControlPoints>(entity)
    );

    UpdateBezierCtrlPtsHandlers(entity, bezierControlPoints);

    // FIXME
    bezierControlPoints.deletionHandler = coordinator->Subscribe<BezierControlPoints>(
        entity,
        std::make_shared<DeletionHandler>(*coordinator)
    );

    coordinator->AddComponent<BezierControlPoints>(entity, bezierControlPoints);
}


void C2CurveSystem::HideBezierControlPoints(Entity entity)
{
    coordinator->EditComponent<C2CurveParameters>(entity,
        [](C2CurveParameters& params) {
            params.showBezierControlPoints = false;
        }
    );

    DeleteBezierControlPoints(entity);
}


void C2CurveSystem::Render() const
{
     if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();

    auto const& shader = shaderRepo->GetBezierShader();
    std::stack<Entity> bSplinePolygonsToDraw;
    std::stack<Entity> bezierPolygonsToDraw;

    UpdateEntities();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& params = coordinator->GetComponent<C2CurveParameters>(entity);
        if (params.drawBSplinePolygon)
            bSplinePolygonsToDraw.push(entity);

        if (params.drawBezierPolygon)
            bezierPolygonsToDraw.push(entity);

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glPatchParameteri(GL_PATCH_VERTICES, 4);
	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

    }

    if (!bSplinePolygonsToDraw.empty())
        RenderBSplinePolygons(bSplinePolygonsToDraw);

    if (!bezierPolygonsToDraw.empty())
        RenderBezierPolygons(bezierPolygonsToDraw);
}


void C2CurveSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        UpdateCurveMesh(entity);

        auto const& params = coordinator->GetComponent<C2CurveParameters>(entity);

        if (params.drawBSplinePolygon)
            UpdateBSplinePolygon(entity);

        if (params.showBezierControlPoints)
            UpdateBezierControlPoints(entity, params);

        coordinator->GetSystem<ToUpdateSystem>()->Unmark(entity);
    }
}


void C2CurveSystem::UpdateCurveMesh(Entity curve) const
{
    coordinator->EditComponent<Mesh>(curve,
        [curve, this](Mesh& mesh) {
            auto const& params = coordinator->GetComponent<CurveControlPoints>(curve);

            mesh.Update(
                GenerateCurveMeshVertices(params),
                GenerateCurveMeshIndices(params)
            );
        }
    );
}


void C2CurveSystem::UpdateBSplinePolygon(Entity curve) const
{
    coordinator->EditComponent<BSplinePolygonMesh>(curve,
        [curve, this](BSplinePolygonMesh& mesh) {
            auto const& params = coordinator->GetComponent<CurveControlPoints>(curve);

            mesh.Update(
                GenerateBSplinePolygonVertices(params),
                GenerateBSplinePolygonIndices(params)
            );
        }
    );
}


void C2CurveSystem::UpdateBezierControlPoints(Entity curve, const C2CurveParameters & params) const
{
    auto const& bezierControlPoints = coordinator->GetComponent<BezierControlPoints>(curve);

    auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(curve);
    auto controlPointsPositions = CreateBezierControlPointsPositions(controlPoints);

    if (controlPointsPositions.size() > bezierControlPoints.Size()) {
        coordinator->EditComponent<BezierControlPoints>(curve,
            [&controlPointsPositions, curve, this](BezierControlPoints& bezier) {
                auto pointsSystem = coordinator->GetSystem<PointsSystem>();

                do {
                    Entity entity = pointsSystem->CreatePoint(Position(0.f));
                    bezier.AddControlPoint(entity);
                } while (controlPointsPositions.size() > bezier.Size());

                this->UpdateBezierCtrlPtsHandlers(curve, bezier);
            }
        );
    }

    if (controlPointsPositions.size() < bezierControlPoints.Size()) {
        coordinator->EditComponent<BezierControlPoints>(curve,
            [&controlPointsPositions, curve, this](BezierControlPoints& bezier) {
                do {
                    auto point = *bezier.ControlPoints().begin();
                    bezier.DeleteControlPoint(point);
                    coordinator->DestroyEntity(point);
                } while (controlPointsPositions.size() < bezier.Size());

                this->UpdateBezierCtrlPtsHandlers(curve, bezier);
            }
        );
    }

    coordinator->EditComponent<BezierControlPoints>(curve,
        [&controlPointsPositions, this](BezierControlPoints& bezier) {
            auto& controlPoints = bezier.ControlPoints();
            
            for (int i=0; i < bezier.Size(); ++i) {
                auto const& oldPos = coordinator->GetComponent<Position>(controlPoints[i]);

                if (oldPos.vec != controlPointsPositions[i])
                    coordinator->SetComponent<Position>(controlPoints[i], Position(controlPointsPositions[i]));
            }
        }
    );
}


void C2CurveSystem::UpdateBezierCtrlPtsHandlers(Entity curve, BezierControlPoints& bezierCtrlPts) const
{
    // FIXME: there is no need for handler deletion

    // Removing all handlers
    auto entitiesIt = bezierCtrlPts.ControlPoints().begin();
    auto handlersIt = bezierCtrlPts.controlPointsHandlers.begin();

    while (handlersIt != bezierCtrlPts.controlPointsHandlers.end() && entitiesIt != bezierCtrlPts.ControlPoints().end()) {
        coordinator->Unsubscribe<Position>(*entitiesIt, (*handlersIt).second);

        ++entitiesIt;
        ++handlersIt;
    }

    if (bezierCtrlPts.Size() < MIN_CTRL_PTS_CNT)
        return;

    auto const& bSplineCtrlPts = coordinator->GetComponent<CurveControlPoints>(curve);

    // Adding new ones
    auto handler = std::make_shared<BezierCtrlPtMovedHandler>(*coordinator, curve);

    for (auto entity: bezierCtrlPts.ControlPoints()) {
        auto handlerID = coordinator->Subscribe<Position>(entity, handler);
        bezierCtrlPts.controlPointsHandlers.insert({ entity, handlerID });
    }
}


void C2CurveSystem::RenderBSplinePolygons(std::stack<Entity>& entities) const
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    while (!entities.empty()) {
        Entity entity = entities.top();
        entities.pop();

        auto const& mesh = coordinator->GetComponent<BSplinePolygonMesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}


void C2CurveSystem::RenderBezierPolygons(std::stack<Entity>& entities) const
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    while (!entities.empty()) {
        Entity entity = entities.top();
        entities.pop();

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

    }
}


BezierControlPoints C2CurveSystem::CreateBezierControlPoints(const CurveControlPoints & params) const
{
    BezierControlPoints bezierControlPoints;
    auto pointsSystem = coordinator->GetSystem<PointsSystem>();

    auto pointsPositions = CreateBezierControlPointsPositions(params);

    for (auto& vec: pointsPositions) {
        Entity point = pointsSystem->CreatePoint(Position(vec));
        bezierControlPoints.AddControlPoint(point);
    }

    return bezierControlPoints;
}


void C2CurveSystem::DeleteBezierControlPoints(Entity entity) const
{
    auto const& bezierControlPoints = coordinator->GetComponent<BezierControlPoints>(entity);

    for (auto point: bezierControlPoints.ControlPoints()) {
        coordinator->DestroyEntity(point);
    }

    coordinator->DeleteComponent<BezierControlPoints>(entity);
}


std::vector<alg::Vec3> C2CurveSystem::CreateBezierControlPointsPositions(const CurveControlPoints & params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<alg::Vec3> result;

    int ctrlPtsCnt = BezierControlPointsCnt(controlPoints.size());
    if (ctrlPtsCnt == 0)
        return result;

    result.reserve(ctrlPtsCnt);

    auto const& pos1 = coordinator->GetComponent<Position>(controlPoints[0]);
    auto const& pos2 = coordinator->GetComponent<Position>(controlPoints[1]);
    auto const& pos3 = coordinator->GetComponent<Position>(controlPoints[2]);

    // Adding the first point
    alg::Vec3 g0 = (1.f/3.f) * pos1.vec + (2.f/3.f) * pos2.vec;
    alg::Vec3 f1 = (2.f/3.f) * pos2.vec + (1.f/3.f) * pos3.vec;
    alg::Vec3 e0 = (g0 + f1) * 0.5f;
    result.push_back(e0);

    for (int i=1; i < controlPoints.size() - 2 ; ++i) {
        auto const& pos1 = coordinator->GetComponent<Position>(controlPoints[i]);
        auto const& pos2 = coordinator->GetComponent<Position>(controlPoints[i+1]);
        auto const& pos3 = coordinator->GetComponent<Position>(controlPoints[i+2]);

        alg::Vec3 f1 = (2.f/3.f) * pos1.vec + (1.f/3.f) * pos2.vec;
        alg::Vec3 g1 = (1.f/3.f) * pos1.vec + (2.f/3.f) * pos2.vec;
        alg::Vec3 f2 = (2.f/3.f) * pos2.vec + (1.f/3.f) * pos3.vec;
        alg::Vec3 e1 = (g1 + f2) * 0.5f;

        result.push_back(f1);
        result.push_back(g1);
        result.push_back(e1);
    }

    return result;
}


std::vector<float> C2CurveSystem::GenerateCurveMeshVertices(const CurveControlPoints& params) const
{
    std::vector<float> result;

    auto controlPointsPositions = CreateBezierControlPointsPositions(params);

    result.reserve(controlPointsPositions.size() * 3);

    for (auto& vec: controlPointsPositions) {
        result.push_back(vec.X());
        result.push_back(vec.Y());
        result.push_back(vec.Z());
    }

    return result;
}


std::vector<uint32_t> C2CurveSystem::GenerateCurveMeshIndices(const CurveControlPoints& params) const
{
    auto const& controlPoints = params.ControlPoints();
    int segments = std::max<int>(controlPoints.size() - 3, 0);
    std::vector<uint32_t> result;
    result.reserve(4*segments);

    for (int i = 0; i < segments*3; i += 3) {
        result.push_back(i);
        result.push_back(i+1);
        result.push_back(i+2);
        result.push_back(i+3);
    }

    return result;
}


std::vector<float> C2CurveSystem::GenerateBSplinePolygonVertices(const CurveControlPoints & params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<float> result;
    result.reserve(3 * controlPoints.size());

    for (Entity entity: controlPoints) {
        auto const& pos = coordinator->GetComponent<Position>(entity);

        result.push_back(pos.GetX());
        result.push_back(pos.GetY());
        result.push_back(pos.GetZ());
    }

    return result;
}


std::vector<uint32_t> C2CurveSystem::GenerateBSplinePolygonIndices(const CurveControlPoints & params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<uint32_t> result(controlPoints.size());

    for (int i = 0; i < result.size(); ++i) {
        result[i] = i;
    }

    return result;
}


void C2CurveSystem::BezierCtrlPtMovedHandler::HandleEvent(Entity entity, const Position & component, EventType eventType)
{
    // FIXME: remove this condition
    if (coordinator.GetEntityComponents(c2Curve).contains(ComponentsManager::GetComponentId<ToUpdate>()))
        return;

    auto const& bSplineCPs = coordinator.GetComponent<CurveControlPoints>(c2Curve).ControlPoints();

    if (bSplineCPs.size() < MIN_CTRL_PTS_CNT)
        return;

    auto const& bezierCPs = coordinator.GetComponent<BezierControlPoints>(c2Curve).ControlPoints();
    auto index = std::find(bezierCPs.begin(), bezierCPs.end(), entity) - bezierCPs.begin();

    if (index >= bezierCPs.size())
        return;

    if (index == 0)
        return FirstCPMoved(bezierCPs, bSplineCPs);

    if (index == bezierCPs.size() - 1)
        return LastCPMoved(bezierCPs, bSplineCPs);

    switch (index % 3)
    {
    case 0:
        SecondFromFullLineMoved(bezierCPs, bSplineCPs, index);
        break;

    case 1:
        ThirdFromFullLineMoved(bezierCPs, bSplineCPs, index);
        break;

    case 2:
        FirstFromFullLineMoved(bezierCPs, bSplineCPs, index);
        break;
    
    default:
        break;
    }
}


void C2CurveSystem::BezierCtrlPtMovedHandler::FirstCPMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs) const
{
    auto const& firstBezierCP = coordinator.GetComponent<Position>(bezierCPs.at(0));
    auto const& secondBezierCP = coordinator.GetComponent<Position>(bezierCPs.at(1));

    auto const& secondBSplineCP = coordinator.GetComponent<Position>(bSplineCPs.at(1));

    auto zeroBezierPos = 2.f * firstBezierCP.vec - secondBezierCP.vec;
    auto firstBSplineNewPos = (zeroBezierPos - secondBSplineCP.vec) * 3.f + secondBSplineCP.vec;

    coordinator.SetComponent<Position>(bSplineCPs.at(0), Position(firstBSplineNewPos));
}


void C2CurveSystem::BezierCtrlPtMovedHandler::LastCPMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs) const
{
    auto const& lastBezierCP = coordinator.GetComponent<Position>(bezierCPs.at(bezierCPs.size() - 1));
    auto const& secondToLastBezierCP = coordinator.GetComponent<Position>(bezierCPs.at(bezierCPs.size() - 2));

    auto const& secondToLastBSplineCP = coordinator.GetComponent<Position>(bSplineCPs.at(bSplineCPs.size() - 2));

    auto afterLastBezierPos = 2.f * lastBezierCP.vec - secondToLastBezierCP.vec;
    auto lastBSplineNewPos = (afterLastBezierPos - secondToLastBSplineCP.vec) * 3.f + secondToLastBSplineCP.vec;

    coordinator.SetComponent<Position>(bSplineCPs.at(bSplineCPs.size() - 1), Position(lastBSplineNewPos));
}


void C2CurveSystem::BezierCtrlPtMovedHandler::FirstFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const
{
    int bSplineCPToModifyIndex = (bezierCtrlPtIndex+1)/3 + 1;

    auto const& bSplineCPToStay = coordinator.GetComponent<Position>(bSplineCPs[bSplineCPToModifyIndex - 1]);
    auto const& newBezierPos = coordinator.GetComponent<Position>(bezierCPs[bezierCtrlPtIndex]);

    auto newPos = (newBezierPos.vec - bSplineCPToStay.vec) * 0.5f + newBezierPos.vec;

    coordinator.SetComponent<Position>(bSplineCPs.at(bSplineCPToModifyIndex), Position(newPos));
}


void C2CurveSystem::BezierCtrlPtMovedHandler::SecondFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const
{
    int bSplineCPToModifyIndex = bezierCtrlPtIndex/3 + 1;

    auto const& prevCP = coordinator.GetComponent<Position>(bSplineCPs[bSplineCPToModifyIndex - 1]);
    auto const& nextCP = coordinator.GetComponent<Position>(bSplineCPs[bSplineCPToModifyIndex + 1]);
    auto const& newBezierPos = coordinator.GetComponent<Position>(bezierCPs[bezierCtrlPtIndex]);

    auto newPos = (3.f*newBezierPos.vec - 0.5f*(prevCP.vec + nextCP.vec)) * 0.5f;

    coordinator.SetComponent<Position>(bSplineCPs.at(bSplineCPToModifyIndex), Position(newPos));
}


void C2CurveSystem::BezierCtrlPtMovedHandler::ThirdFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const
{
    int bSplineCPToModifyIndex = (bezierCtrlPtIndex-1)/3 + 1;

    auto const& bSplineCPToStay = coordinator.GetComponent<Position>(bSplineCPs[bSplineCPToModifyIndex + 1]);
    auto const& newBezierPos = coordinator.GetComponent<Position>(bezierCPs[bezierCtrlPtIndex]);

    auto newPos = (newBezierPos.vec - bSplineCPToStay.vec) * 0.5f + newBezierPos.vec;

    coordinator.SetComponent<Position>(bSplineCPs.at(bSplineCPToModifyIndex), Position(newPos));
}


// FIXME: delete code duplication
void C2CurveSystem::DeletionHandler::HandleEvent(Entity entity, const BezierControlPoints & component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto entitiesIt = component.ControlPoints().begin();
    auto handlersIt = component.controlPointsHandlers.begin();

    while (handlersIt != component.controlPointsHandlers.end() && entitiesIt != component.ControlPoints().end()) {
        coordinator.Unsubscribe<Position>(*entitiesIt, (*handlersIt).second);

        ++entitiesIt;
        ++handlersIt;
    }

    coordinator.Unsubscribe<CurveControlPoints>(entity, component.deletionHandler);
}
