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

void PhysicsEngine::handleProjectileEnvironmentHit(
    size_t projectileIdx, PhysicsBody& body, const sf::Vector2f& moment)
{
    assert(scene.actors[projectileIdx].inventoryIdx.has_value());
    const auto inventoryIdx = *scene.actors[projectileIdx].inventoryIdx;

    assert(std::holds_alternative<ProjectileInventory>(
        scene.inventories[inventoryIdx]));

    auto&& inventory =
        std::get<ProjectileInventory>(scene.inventories[inventoryIdx]);

    if (inventory.traits == ProjectileTraits::Bouncy)
    {
        if (moment.x == 0.f) body.forward.x *= -1.f;
        if (moment.y == 0.f) body.forward.y *= -1.f;
    }
    else
    {
        eventQueue.pushEvent<event::ProjectileHitSomething>(
            projectileIdx, std::nullopt);
    }
}
