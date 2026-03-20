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
        {
            createDamageMarkerForProjectile(e.entity, inventory);
            eventQueue.pushEvent<event::ProjectileDestroyed>(e.entity);
        }
    }
}

void GameRulesEngine::operator()(const event::ActorToActorCollision& e)
{
    // NOTE: See PhysicsEngine's invariants - projectiles and damage
    // markers should always be the second entity in collision event.
    handleProjectileToActorCollision(e.entity2, e.entity1);
    handleDamageMarkerToActorCollision(e.entity2, e.entity1);
    handleTriggerToActorCollision(e.entity2, e.entity1);
}

void GameRulesEngine::operator()(const event::ActorStartedAttack& e)
{
    auto& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == ActorKind::Player)
    {
        eventQueue.pushEvent<event::ActorFiredWeapon>(e.entity);
    }
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
    soundPlayer.playPovSound(weapon.soundId);

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

void GameRulesEngine::operator()(const event::DoorOpened& e)
{
    auto&& collider = scene.actors.get<Collider>(e.doorEntity);
    collider.options.disabled = true;
    collider.options.nonblocking = true;
}

void GameRulesEngine::operator()(const event::DoorStartsClosing& e)
{
    auto&& collider = scene.actors.get<Collider>(e.doorEntity);
    collider.options.disabled = false;
    collider.options.nonblocking = false;
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
            true ? SkinType::Scuttlebug : SkinType::Bighead,
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
            eventQueue.pushEvent<event::ActorStartedAttack>(entity);
        }
        else if (controller->isSwapWeaponsPressed())
        {
            weaponInventory.activeWeapon = 1 - weaponInventory.activeWeapon;
        }

        weapon.timeTillFire -= time.getElapsed();
    }

    updateLifetimes(time);

    for (auto&& [entity, health] : scene.actors.view<Health>().each())
    {
        if (health.get() <= 0)
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
    }

    for (auto&& [entity, inventory] :
         scene.actors.view<TriggerInventory>().each())
    {
        // if (inventory.delay <= sf::Time::Zero) continue;
        inventory.delay -= time.getElapsed();

        if (inventory.delay <= sf::Time::Zero)
        {
            auto& targetSkin = scene.actors.get<Skin>(inventory.targetEntity);
            if (targetSkin.animation.getStateName()
                == DOOR_OPEN_ANIMATION_STATE)
            {
                targetSkin.animation.setState(
                    DOOR_CLOSING_ANIMATION_STATE, "looping"_false);
                eventQueue.pushEvent<event::DoorStartsClosing>(
                    inventory.targetEntity);
            }
        }
    }
}

void GameRulesEngine::updateLifetimes(const dgm::Time& time)
{
    for (auto&& [entity, lifetime] : scene.actors.view<Lifetime>().each())
    {
        lifetime.get() -= time.getElapsed();
        if (lifetime.get() <= sf::Time::Zero)
        {
            if (auto inventory =
                    scene.actors.try_get<ProjectileInventory>(entity))
            {
                createDamageMarkerForProjectile(entity, inventory);
            }

            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
        }
    }
}

void GameRulesEngine::handleProjectileToActorCollision(
    entt::entity projectile, entt::entity actor)
{
    auto inventory = scene.actors.try_get<ProjectileInventory>(projectile);
    if (!inventory) return;

    auto skin = scene.actors.try_get<Skin>(actor);
    if (!skin) return;

    if (skin->kind == inventory->originator) return;

    createDamageMarkerForProjectile(projectile, inventory);

    if (inventory->traits & ProjectileTraits::Passthru) return;

    eventQueue.pushEvent<event::ProjectileDestroyed>(projectile);
}

void GameRulesEngine::createDamageMarkerForProjectile(
    entt::entity projectile, ProjectileInventory* inventory)
{
    ActorBuilder::createDamageMarker(
        scene.actors,
        scene.actors.get<Collider>(projectile).getPosition(),
        inventory->traits & ProjectileTraits::Explosive
            ? BASE_EXPLOSION_RADIUS
            : scene.actors.get<Collider>(projectile).getRadius(),
        *inventory);
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
    skin->animation.setState(HURT_ANIMATION_STATE.data(), "looping"_false);
}

void GameRulesEngine::handleTriggerToActorCollision(
    entt::entity triggerIdx, entt::entity actorIdx)
{
    auto&& inventory = scene.actors.try_get<TriggerInventory>(triggerIdx);
    if (!inventory) return;

    // Nobody except for the player can open the doors
    auto skin = scene.actors.try_get<Skin>(actorIdx);
    if (!skin || skin->kind != ActorKind::Player) return;

    inventory->delay = BASE_DOOR_CLOSE_DELAY;
    auto&& targetSkin = scene.actors.get<Skin>(inventory->targetEntity);

    if (targetSkin.animation.getStateName() == DOOR_CLOSED_ANIMATION_STATE)
    {
        targetSkin.animation.setState(
            DOOR_OPENING_ANIMATION_STATE, "looping"_false);
    }
}
