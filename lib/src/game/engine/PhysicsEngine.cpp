#include "game/engine/PhysicsEngine.hpp"

void PhysicsEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, idx] : scene.actors)
    {
        std::visit(
            [&](const auto& collider)
            { updateForConcreteCollider(time, actor, idx, collider); },
            actor.body.shape);
    }
}

void PhysicsEngine::handleProjectileHit(
    size_t projectileIdx, std::optional<size_t> hitActorIdx)
{
    assert(scene.actors[projectileIdx].inventoryIdx.has_value());
    const auto inventoryIdx = *scene.actors[projectileIdx].inventoryIdx;

    assert(std::holds_alternative<ProjectileInventory>(
        scene.inventories[inventoryIdx]));

    auto&& inventory =
        std::get<ProjectileInventory>(scene.inventories[inventoryIdx]);

    /* TODO: bouncy projectile
    if (inventory.traits & ProjectileTraits::Bounce)
    {
        body.forward *= -body.bounciness;
    }*/

    eventQueue.pushEvent<event::ProjectileHitSomething>(
        projectileIdx, hitActorIdx);
}
