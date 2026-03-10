#include "game/engine/GameRulesEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "game/builders/GameSceneBuilder.hpp"
#include <algorithm>
#include <limits>
#include <print>

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
    // NOTE: See PhysicsEngine's invariants - projectiles and damage
    // markers should always be the second entity in collision event.
    handleProjectileToActorCollision(e.entity2, e.entity1);
    handleDamageMarkerToActorCollision(e.entity2, e.entity1);
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
    if (weapon.numShots == 1)
        soundPlayer.playPovSound(SoundId::Bullet);
    else
        soundPlayer.playPovSound(SoundId::Shotgun);

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
    auto&& inventory =
        scene.actors.get<ProjectileInventory>(e.projectileEntity);

    const auto distance =
        (scene.actors.get<Collider>(e.projectileEntity).getPosition()
         - scene.actors.get<Collider>(scene.playerEntity).getPosition())
            .length();
    if (inventory.traits & ProjectileTraits::Explosive)
    {
        soundPlayer.playAttenuatedSound(
            SoundChannel::Ambient, SoundId::Explosion, distance);
    }

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
        scene.spawnTicker = sf::Time::Zero;
        ActorBuilder::createNpc(
            scene.actors,
            scene.enemySpawns[rand() % scene.enemySpawns.size()],
            scene,
            atlas);
    }

    for (auto&& [entity, controller, body, lookDirection, weaponInventory] :
         scene.actors
             .view<EntityInput, PhysicsBody, LookDirection, WeaponInventory>()
             .each())
    {
        auto&& forwardImpulse = controller->getForward();
        if (forwardImpulse.length() > 0.f)
            body.forward += forwardImpulse * body.maxSpeed;

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
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
        }
    }

    for (auto&& [entity, health] : scene.actors.view<Health>().each())
    {
        if (health.get() <= 0)
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
    }
}

void GameRulesEngine::handleProjectileToActorCollision(
    entt::entity projectile, entt::entity actor)
{
    auto inventory = scene.actors.try_get<ProjectileInventory>(projectile);
    if (!inventory) return;

    auto skin = scene.actors.try_get<Skin>(actor);
    if (!skin) return;

    // TODO: implement projectile originator
    if (skin->kind == ActorKind::Player) return;

    ActorBuilder::createDamageMarker(
        scene.actors,
        scene.actors.get<Collider>(projectile).getPosition(),
        inventory->traits & ProjectileTraits::Explosive
            ? BASE_EXPLOSION_RADIUS
            : scene.actors.get<Collider>(projectile).getRadius(),
        *inventory);

    if (inventory->traits & ProjectileTraits::Passthru) return;

    eventQueue.pushEvent<event::ProjectileDestroyed>(projectile);
}

void GameRulesEngine::handleDamageMarkerToActorCollision(
    entt::entity marker, entt::entity actor)
{
    auto inventory = scene.actors.try_get<DamageMarkerInventory>(marker);
    if (!inventory) return;

    auto [skin, health] = scene.actors.try_get<Skin, Health>(actor);
    if (!skin || !health) return;

    if (skin->kind == inventory->originator) return;

    health->get() -= inventory->damage;
}
