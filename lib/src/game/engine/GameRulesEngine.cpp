#include "game/engine/GameRulesEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "game/builders/GameSceneBuilder.hpp"
#include <algorithm>
#include <limits>

void GameRulesEngine::operator()(const event::ActorToMeshCollision& e)
{
    if (auto inventory = scene.actors.try_get<ProjectileInventory>(e.entity))
    {
        if (!(inventory->traits & ProjectileTraits::Bouncy))
            eventQueue.pushEvent<event::ProjectileDestroyed>(e.entity);
    }
}

void GameRulesEngine::operator()(const event::ActorToActorCollision& e)
{
    /*const bool isActor1Projectile =
        scene.actors[e.actor1].kind == ActorKind::Projectile;
    const bool isActor2Projectile =
        scene.actors[e.actor2].kind == ActorKind::Projectile;
    const bool isActor1DamageMarker =
        scene.actors[e.actor1].kind == ActorKind::DamageMarker;
    const bool isActor2DamageMarker =
        scene.actors[e.actor2].kind == ActorKind::DamageMarker;

    if (isActor1Projectile && isActor2Projectile) return;
    if (isActor1DamageMarker && isActor2DamageMarker) return;

    if ((isActor1Projectile || isActor2Projectile)
        && (isActor1DamageMarker || isActor2DamageMarker))
        return;

    if (isActor1Projectile || isActor2Projectile)
        handleProjectileToActorCollision(
            isActor1Projectile ? e.actor1 : e.actor2,
            isActor1Projectile ? e.actor2 : e.actor1);
    else if (isActor1DamageMarker || isActor2DamageMarker)
        handleDamageMarkerToActorCollision(
            isActor1DamageMarker ? e.actor1 : e.actor2,
            isActor1DamageMarker ? e.actor2 : e.actor1);*/
}

void GameRulesEngine::operator()(const event::ActorFiredWeapon& e)
{
    auto&& [lookDirection, shooterCollider, shooterBody, weaponInventory] =
        scene.actors.get<LookDirection, Collider, PhysicsBody, WeaponInventory>(
            e.entity);

    auto&& weapon = getActiveWeapon(weaponInventory);
    assert(weapon.timeTillFire <= sf::Time::Zero);
    weapon.timeTillFire = weapon.cooldown;

    // TODO: might not be POV
    soundPlayer.playPovSound(SoundId::Bullet);

    for (auto&& _ : std::views::iota(0, weapon.numShots))
    {
        const auto spread = rand() % (weapon.spread * 2) - weapon.spread;

        const auto direction = lookDirection.get().rotatedBy(
            sf::degrees(static_cast<float>(spread)));

        auto projectile = ActorBuilder::createProjectile(
            scene.actors,
            shooterCollider.getPosition(),
            dgm::Math::toUnit(direction),
            atlas,
            weapon);

        auto&& projectileCollider = scene.actors.get<Collider>(projectile);

        const auto spawnOffset = dgm::Math::toUnit(direction)
                                 * (projectileCollider.getRadius()
                                    + shooterCollider.getRadius() + 1.f);
        projectileCollider.move(spawnOffset);
    }

    shooterBody.forward += -lookDirection.get() * weapon.kickback;
}

void GameRulesEngine::operator()(const event::ProjectileDestroyed& e)
{
    eventQueue.pushEvent<event::ObjectDestroyed>(e.projectileEntity);
}

void GameRulesEngine::operator()(const event::EnemyAttackLands& e)
{
    // TODO: create short-lived projectile
    /*const auto& actor = scene.actors[e.enemyIdx];
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
        })));*/
}

void GameRulesEngine::update(const dgm::Time& time)
{
    scene.spawnTicker += time.getElapsed();
    if (scene.spawnTicker > scene.spawnDelay)
    {
        /*
        scene.spawnTicker = sf::Time::Zero;
        scene.actors.emplaceBack(ActorBuilder::createNpc(
            scene.enemySpawns[rand() % scene.enemySpawns.size()],
            atlas,
            scene.inventories.emplaceBack(
                GameSceneBuilder::createNpcInventory())));*/
    }

    for (auto&& [entity, controller, body, lookDirection, weaponInventory] :
         scene.actors
             .view<EntityInput, PhysicsBody, LookDirection, WeaponInventory>()
             .each())
    {
        auto&& forwardImpulse = controller->getForward();
        if (forwardImpulse.length() > 0.f)
            body.forward += forwardImpulse * BASE_PLAYER_SPEED;

        const auto direction = controller->getAimDirection();
        if (direction.length() > 0.f) lookDirection = direction;

        auto&& weapon = getActiveWeapon(weaponInventory);
        if (controller->isShootPressed()
            && weapon.timeTillFire <= sf::Time::Zero)
        {
            eventQueue.pushEvent<event::ActorFiredWeapon>(entity);
        }
        else if (controller->isSwapWeaponsPressed())
        {
            weaponInventory.activeWeapon = 1 - weaponInventory.activeWeapon;
        }

        weapon.timeTillFire -= time.getElapsed();
    }

    for (auto&& [entity, lifetime] : scene.actors.view<Lifetime>().each())
    {
        lifetime.get() -= time.getElapsed();
        if (lifetime.get() <= sf::Time::Zero)
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
    }
}

/*
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
}*/

void GameRulesEngine::handleProjectileToActorCollision(
    ActorIndexType projectileIdx, ActorIndexType actorIdx)
{
    /*if (scene.actors[actorIdx].kind == ActorKind::Player) return;

    auto&& [projectile, projectileInventory] =
        getActorAndInventory<ProjectileInventory>(scene, projectileIdx);

    // Spawn damage marker
    scene.actors.emplaceBack(ActorBuilder::createDamageMarker(
        projectile.body.getPosition(),
        projectileInventory.traits & ProjectileTraits::Explosive
            ? BASE_EXPLOSION_RADIUS
            : projectile.body.getRadius(),
        scene.inventories.emplaceBack(DamageMarkerInventory {
            .originator = ActorKind::Player,
            .damage = projectileInventory.damage,
        })));

    if (projectileInventory.traits & ProjectileTraits::Passthru) return;

    eventQueue.pushEvent<event::ProjectileDestroyed>(projectileIdx);*/
}

void GameRulesEngine::handleDamageMarkerToActorCollision(
    ActorIndexType markerIdx, ActorIndexType actorIdx)
{
    /*auto&& markerInventoryIdx = scene.actors[markerIdx].inventoryIdx;
    assert(markerInventoryIdx);
    auto&& markerInventory =
        std::get<DamageMarkerInventory>(scene.inventories[*markerInventoryIdx]);

    auto&& actor = scene.actors[actorIdx];

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
        scene.inventories[*actor.inventoryIdx]);*/
}
