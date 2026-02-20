#include "game/engine/GameRulesEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include <algorithm>
#include <limits>

const float SPEED = 192.f;

void GameRulesEngine::operator()(const event::PlayerFiredWeapon&)
{
    assert(scene.actors.isIndexValid(0));
    assert(scene.actors[0].kind == ActorKind::Player);
    assert(scene.inventories.isIndexValid(0));
    assert(std::holds_alternative<PlayerInventory>(scene.inventories[0]));

    auto&& inventory = std::get<PlayerInventory>(scene.inventories[0]);

    assert(inventory.weapon.timeTillFire <= sf::Time::Zero);
    inventory.weapon.timeTillFire = inventory.weapon.cooldown;

    auto&& player = scene.actors[0];

    for (auto&& _ : std::views::iota(0, inventory.weapon.numShots))
    {
        auto spread =
            rand() % (inventory.weapon.spread * 2) - inventory.weapon.spread;

        scene.actors.emplaceBack(ActorBuilder::createProjectile(
            player.body.getPosition(),
            player.lookDirection.rotatedBy(
                sf::degrees(static_cast<float>(spread))),
            atlas,
            scene.inventories.emplaceBack(ProjectileInventory {})));
    }

    player.body.forward += -player.lookDirection * inventory.weapon.kickback;
}

void GameRulesEngine::operator()(const event::ProjectileHitSomething& e)
{
    auto&& projectile = scene.actors[e.projectileIdx];
    auto&& collider = std::get<dgm::Circle>(projectile.body.shape);
    auto&& projectileInventory = std::get<ProjectileInventory>(
        scene.inventories[*projectile.inventoryIdx]);

    for (auto&& [actor, idx] : scene.actors)
    {
        if (actor.kind == ActorKind::Npc && actor.body.collidesWith(collider))
        {
            auto inventory = std::get<NpcInventory>(
                scene.inventories[actor.inventoryIdx.value()]);
            inventory.health -= projectileInventory.damage;
        }
    }
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
        actor.body.forward += forwardImpulse * SPEED;

    auto&& direction = input.getAimDirection();
    if (direction.length() > 0.f) actor.lookDirection = direction;

    scene.cameraPosition = actor.body.getPosition();

    if (input.isShootPressed()
        && inventory.weapon.timeTillFire <= sf::Time::Zero)
    {
        eventQueue.pushEvent<event::PlayerFiredWeapon>();
    }

    inventory.weapon.timeTillFire -= time.getElapsed();
}

void GameRulesEngine::updateProjectile(
    Actor&, ProjectileInventory&, const dgm::Time&)
{
}
