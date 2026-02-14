#include "game/engine/GameRulesEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include <algorithm>
#include <limits>

const float SPEED = 192.f;

void GameRulesEngine::operator()(const event::PlayerWantsToFire&)
{
    assert(scene.actors.isIndexValid(0));
    assert(scene.actors[0].kind == ActorKind::Player);
    assert(scene.inventories.isIndexValid(0));
    assert(std::holds_alternative<PlayerInventory>(scene.inventories[0]));
    auto&& inventory = std::get<PlayerInventory>(scene.inventories[0]);

    if (inventory.weapon.timeTillFire > sf::Time::Zero) return;

    inventory.weapon.timeTillFire = inventory.weapon.cooldown;

    auto& player = scene.actors[0];
    auto&& actorIdx = scene.actors.emplaceBack(ActorBuilder::createProjectile(
        player.body.getPosition(),
        player.lookDirection,
        atlas,
        scene.inventories.emplaceBack(ProjectileInventory {})));
}

void GameRulesEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, _] : scene.actors)
    {
        assert(actor.kind != ActorKind::None);

        if (actor.kind == ActorKind::Player)
        {
            assert(actor.inventoryIdx);
            assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
            updatePlayer(
                actor,
                std::get<PlayerInventory>(
                    scene.inventories[*actor.inventoryIdx]),
                time);
        }
        else if (actor.kind == ActorKind::Projectile)
        {
            assert(actor.inventoryIdx);
            assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
            updateProjectile(
                actor,
                std::get<ProjectileInventory>(
                    scene.inventories[*actor.inventoryIdx]),
                time);
        }
    }
}

void GameRulesEngine::updatePlayer(
    Actor& actor, PlayerInventory& inventory, const dgm::Time& time)
{
    auto&& forwardImpulse = input.getForward();
    if (forwardImpulse.length() > 0.f)
        actor.body.forward = forwardImpulse * SPEED;

    auto&& direction = input.getAimDirection();
    if (direction.length() > 0.f) actor.lookDirection = direction;

    scene.cameraPosition = actor.body.getPosition();

    if (input.isShootPressed())
    {
        eventQueue.pushEvent<event::PlayerWantsToFire>();
    }

    inventory.weapon.timeTillFire -= time.getElapsed();
}

void GameRulesEngine::updateProjectile(
    Actor&, ProjectileInventory&, const dgm::Time&)
{
}
