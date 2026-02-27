#include "game/engine/GameRulesEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "game/builders/GameSceneBuilder.hpp"
#include <algorithm>
#include <limits>

void GameRulesEngine::operator()(const event::PlayerFiredWeapon&)
{
    auto&& [player, inventory] = getActorAndInventory<PlayerInventory>(0);

    auto&& weapon = getPlayerWeapon(inventory);
    assert(weapon.timeTillFire <= sf::Time::Zero);
    weapon.timeTillFire = weapon.cooldown;

    soundPlayer.playPovSound(SoundId::Bullet);

    for (auto&& _ : std::views::iota(0, weapon.numShots))
    {
        const auto spread = rand() % (weapon.spread * 2) - weapon.spread;

        const auto direction = player.lookDirection.rotatedBy(
            sf::degrees(static_cast<float>(spread)));

        auto actor = ActorBuilder::createProjectile(
            player.body.getPosition(),
            dgm::Math::toUnit(direction),
            atlas,
            scene.inventories.emplaceBack(weapon.defaultProjectileInventory));

        const auto spawnOffset =
            dgm::Math::toUnit(direction)
            * (std::get<dgm::Circle>(player.body.shape).getRadius()
               + std::get<dgm::Circle>(actor.body.shape).getRadius() + 1.f);

        actor.body.move(spawnOffset);

        scene.actors.emplaceBack(std::move(actor));
    }

    player.body.forward += -player.lookDirection * weapon.kickback;
}

void GameRulesEngine::operator()(const event::ProjectileHitSomething& e)
{
    const auto&& [projectile, projectileInventory] =
        getActorAndInventory<ProjectileInventory>(e.projectileIdx);

    scene.actors.emplaceBack(ActorBuilder::createDamageMarker(
        projectile.body.getPosition(),
        projectileInventory.traits & ProjectileTraits::Explosive
            ? BASE_EXPLOSION_RADIUS
            : projectile.body.getRadius(),
        scene.inventories.emplaceBack(DamageMarkerInventory {
            .originator = ActorKind::Player,
            .damage = projectileInventory.damage,
        })));

    const bool shouldNotDeleteProjectile =
        projectileInventory.traits & ProjectileTraits::Passthru
        && e.hitActorIdx;

    if (!shouldNotDeleteProjectile)
        eventQueue.pushEvent<event::ObjectDestroyed>(e.projectileIdx);
}

void GameRulesEngine::operator()(const event::EnemyAttackLands& e)
{
    const auto& actor = scene.actors[e.enemyIdx];
    assert(actor.inventoryIdx);
    const auto& inventory =
        std::get<NpcInventory>(scene.inventories[*actor.inventoryIdx]);

    const auto hitboxRadius = 3.f;
    const auto actorDirectionOffset =
        actor.lookDirection * (hitboxRadius + actor.body.getRadius());

    scene.actors.emplaceBack(ActorBuilder::createDamageMarker(
        actor.body.getPosition() + actorDirectionOffset,
        hitboxRadius,
        scene.inventories.emplaceBack(DamageMarkerInventory {
            .damage = inventory.damage,
        })));
}

void GameRulesEngine::operator()(const event::ActorDamaged& e)
{
    auto&& markerInventoryIdx = scene.actors[e.markerIdx].inventoryIdx;
    assert(markerInventoryIdx);
    auto&& markerInventory =
        std::get<DamageMarkerInventory>(scene.inventories[*markerInventoryIdx]);

    auto&& actor = scene.actors[e.hitActorIdx];

    // Ignore non-player, non-npc collisions
    if (actor.kind != ActorKind::Player && actor.kind != ActorKind::Npc) return;
    // Ignore friendly-fire
    if (actor.kind == markerInventory.originator) return;

    assert(actor.inventoryIdx);
    assert(scene.inventories.isIndexValid(*actor.inventoryIdx));

    std::visit(
        overloads {
            [&](PlayerInventory& inventory)
            { inventory.health -= markerInventory.damage; },
            [&](NpcInventory& inventory)
            { inventory.health -= markerInventory.damage; },
            [&](auto&) { assert(false); },
        },
        scene.inventories[*actor.inventoryIdx]);
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
        else if (actor.kind == ActorKind::DamageMarker)
            eventQueue.pushEvent<event::ObjectDestroyed>(idx);
    }
}

void GameRulesEngine::updatePlayer(
    ActorIndexType,
    Actor& actor,
    PlayerInventory& inventory,
    const dgm::Time& time)
{
    auto&& forwardImpulse = input.getForward();
    if (forwardImpulse.length() > 0.f)
        actor.body.forward += forwardImpulse * BASE_PLAYER_SPEED;

    auto&& direction = input.getAimDirection();
    if (direction.length() > 0.f) actor.lookDirection = direction;

    scene.cameraPosition = actor.body.getPosition();

    auto&& weapon = getPlayerWeapon(inventory);
    if (input.isShootPressed() && weapon.timeTillFire <= sf::Time::Zero)
    {
        eventQueue.pushEvent<event::PlayerFiredWeapon>();
    }
    else if (input.isSwapWeaponsPressed())
    {
        inventory.activeWeapon = !inventory.activeWeapon;
    }

    weapon.timeTillFire -= time.getElapsed();
}

void GameRulesEngine::updateNpc(
    ActorIndexType actorIdx,
    Actor& actor,
    NpcInventory& inventory,
    const dgm::Time&)
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
            dgm::Math::toUnit(directionToPlayer) * BASE_ENEMY_SPEED;
        actor.lookDirection = dgm::Math::toUnit(directionToPlayer);
    }
    else if (actor.animation.getStateName() == "walk-front")
    {
        eventQueue.pushEvent<event::EnemyStartedAttack>(actorIdx);
    }
}

void GameRulesEngine::updateProjectile(
    ActorIndexType idx,
    Actor&,
    ProjectileInventory& inventory,
    const dgm::Time& dt)
{
    inventory.lifetime -= dt.getElapsed();
    if (inventory.lifetime <= sf::Time::Zero)
        eventQueue.pushEvent<event::ObjectDestroyed>(idx);
}
