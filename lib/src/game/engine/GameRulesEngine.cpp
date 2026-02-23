#include "game/engine/GameRulesEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "game/builders/GameSceneBuilder.hpp"
#include <algorithm>
#include <limits>

const float SPEED = 192.f;

void GameRulesEngine::operator()(const event::PlayerFiredWeapon&)
{
    // assert(scene.actors.isIndexValid(0));
    assert(scene.actors[0].kind == ActorKind::Player);
    assert(scene.inventories.isIndexValid(0));
    assert(std::holds_alternative<PlayerInventory>(scene.inventories[0]));

    auto&& inventory = std::get<PlayerInventory>(scene.inventories[0]);

    assert(inventory.weapon.timeTillFire <= sf::Time::Zero);
    inventory.weapon.timeTillFire = inventory.weapon.cooldown;

    soundPlayer.playPovSound(SoundId::Bullet);

    auto&& player = scene.actors[0];

    for (auto&& _ : std::views::iota(0, inventory.weapon.numShots))
    {
        const auto spread =
            rand() % (inventory.weapon.spread * 2) - inventory.weapon.spread;

        const auto direction = player.lookDirection.rotatedBy(
            sf::degrees(static_cast<float>(spread)));

        auto actor = ActorBuilder::createProjectile(
            player.body.getPosition(),
            dgm::Math::toUnit(direction),
            atlas,
            scene.inventories.emplaceBack(
                inventory.weapon.defaultProjectileInventory));

        const auto spawnOffset =
            dgm::Math::toUnit(direction)
            * (std::get<dgm::Circle>(player.body.shape).getRadius()
               + std::get<dgm::Circle>(actor.body.shape).getRadius() + 1.f);

        actor.body.move(spawnOffset);

        scene.actors.emplaceBack(std::move(actor));
    }

    player.body.forward += -player.lookDirection * inventory.weapon.kickback;
}

void GameRulesEngine::operator()(const event::ProjectileHitSomething& e)
{
    // TODO: explosion hitbox

    if (e.hitActorIdx)
    {
        auto&& projectile = scene.actors[e.projectileIdx];
        auto&& projectileInventory = std::get<ProjectileInventory>(
            scene.inventories[*projectile.inventoryIdx]);

        auto&& actor = scene.actors[*e.hitActorIdx];

        if (actor.inventoryIdx)
        {
            std::visit(
                overloads {
                    [&](PlayerInventory&) { /* tbd */ },
                    [&](NpcInventory& inventory)
                    { inventory.health -= projectileInventory.damage; },
                    [&](auto&) {},
                },
                scene.inventories[*actor.inventoryIdx]);
        }
    }

    eventQueue.pushEvent<event::ObjectDestroyed>(e.projectileIdx);
}

void GameRulesEngine::operator()(const event::EnemyAttackLands& e)
{
    const auto& actor = scene.actors[e.enemyIdx];
    auto&& attackArea = dgm::Circle(actor.body.getPosition(), 3.f);
    attackArea.move(
        actor.lookDirection
        * (3.f + std::get<dgm::Circle>(actor.body.shape).getRadius()));

    if (scene.actors[0].body.collidesWith(attackArea))
    {
        // TODO: damage player
    }
}

void GameRulesEngine::update(const dgm::Time& time)
{
    scene.spawnTicker += time.getElapsed();
    if (scene.spawnTicker > scene.spawnDelay)
    {
        scene.spawnTicker = sf::Time::Zero;
        scene.actors.emplaceBack(ActorBuilder::createNpc(
            scene.enemySpawns[rand() % scene.enemySpawns.size()],
            atlas,
            scene.inventories.emplaceBack(
                GameSceneBuilder::createNpcInventory())));
    }

    for (auto&& [actor, idx] : scene.actors)
    {
        assert(actor.kind != ActorKind::None);

        if (actor.kind == ActorKind::Player)
        {
            assert(actor.inventoryIdx);
            assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
            updatePlayer(
                idx,
                actor,
                std::get<PlayerInventory>(
                    scene.inventories[*actor.inventoryIdx]),
                time);
        }
        else if (actor.kind == ActorKind::Npc)
        {
            assert(actor.inventoryIdx);
            assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
            updateNpc(
                idx,
                actor,
                std::get<NpcInventory>(scene.inventories[*actor.inventoryIdx]),
                time);
        }
        else if (actor.kind == ActorKind::Projectile)
        {
            assert(actor.inventoryIdx);
            assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
            updateProjectile(
                idx,
                actor,
                std::get<ProjectileInventory>(
                    scene.inventories[*actor.inventoryIdx]),
                time);
        }
    }
}

void GameRulesEngine::updatePlayer(
    size_t, Actor& actor, PlayerInventory& inventory, const dgm::Time& time)
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

void GameRulesEngine::updateNpc(
    size_t actorIdx, Actor& actor, NpcInventory& inventory, const dgm::Time&)
{
    if (inventory.health <= 0)
    {
        eventQueue.pushEvent<event::ObjectDestroyed>(actorIdx);
    }

    auto playerPos = scene.actors[0].body.getPosition();
    auto directionToPlayer = playerPos - actor.body.getPosition();

    if (directionToPlayer.length() > 20.f)
    {
        actor.body.forward =
            dgm::Math::toUnit(directionToPlayer) * SPEED * 0.75f;
        actor.lookDirection = dgm::Math::toUnit(directionToPlayer);
    }
    else if (actor.animation.getStateName() == "walk-front")
    {
        eventQueue.pushEvent<event::EnemyStartedAttack>(actorIdx);
    }
}

void GameRulesEngine::updateProjectile(
    size_t, Actor&, ProjectileInventory&, const dgm::Time&)
{
}
