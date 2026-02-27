#include "game/engine/PhysicsEngine.hpp"

void PhysicsEngine::update(const dgm::Time& time)
{
    spatialIndex.clear();
    for (auto&& [actor, idx] : scene.actors)
    {
        if (actor.kind == ActorKind::Effect) continue;

        std::visit(
            [&](const auto& collider)
            { spatialIndex.returnToLookup(idx, collider); },
            actor.body.shape);
    }

    for (auto&& [actor, idx] : scene.actors)
    {
        std::visit(
            [&](const auto& collider)
            { updateForConcreteCollider(time, actor, idx, collider); },
            actor.body.shape);
    }
}
