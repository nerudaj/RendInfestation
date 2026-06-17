#include "appstate/Game/engine/GameRulesEngine.hpp"
#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/builders/GameSceneBuilder.hpp"
#include "appstate/Game/definitions/Constants.hpp"
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
    if (skin.kind == EntityKind::Player)
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

    const auto unitDirection = dgm::Math::toUnit(lookDirection.get());
    for (auto&& _ : std::views::iota(0, weapon.numShots))
    {
        const auto spread = rand() % (weapon.spread * 2) - weapon.spread;

        const auto direction =
            unitDirection.rotatedBy(sf::degrees(static_cast<float>(spread)));

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

    shooterBody.forward += -unitDirection * weapon.kickback;
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

    if (inventory.traits & ProjectileTraits::SplitOnHit)
    {
        ActorBuilder::shatterProjectile(scene.actors, e.projectileEntity);
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

void GameRulesEngine::operator()(const event::ActorIsFalling& e)
{
    auto& skin = scene.actors.get<Skin>(e.entity);
    skin.scale -= FALL_SPEED * e.deltaTime;
    if (skin.scale <= 0.f)
    {
        skin.scale = 0.f;
        eventQueue.pushEvent<event::ObjectDestroyed>(e.entity);
    }

    if (auto&& einput = scene.actors.try_get<EntityInput>(e.entity))
    {
        einput->get()->freeze();
    }
}

void GameRulesEngine::operator()(const event::ObjectDestroyed& e)
{
    auto skin = scene.actors.try_get<Skin>(e.entity);
    if (skin && skin->kind == EntityKind::Npc)
    {
        if (scene.survivalGameDirector)
            scene.status.score +=
                scene.survivalGameDirector->markKilledEnemy(*skin);

        ActorBuilder::createParticleSystem(
            scene.actors,
            scene.actors.get<Collider>(e.entity).getPosition(),
            sf::Vector2f { 0.f, -1.f },
            ParticleSystemKind::BloodSpatter);

        if (skin->skinType == SkinType::Scuttlebug)
            soundPlayer.playAttenuatedSound(
                SoundChannel::Enemy,
                SoundId::ScuttlebugDeath,
                (scene.actors.get<Collider>(e.entity).getPosition()
                 - scene.actors.get<Collider>(scene.playerEntity).getPosition())
                    .length());
    }
    // turrets also have entity kind player
    else if (
        skin && skin->skinType == SkinType::PlayerDefault
        && skin->kind == EntityKind::Player)
    {
        // todo: event
        scene.status.finished = true;
    }
}

void GameRulesEngine::operator()(const event::SurvivalSpawnerTimerHit& e)
{
    ActorBuilder::createNpc(
        scene.actors, pickEnemySpawnPosition(), e.typeToSpawn, atlas);
}

void GameRulesEngine::operator()(const event::ActorDamaged& e)
{
    auto&& [health, body] = scene.actors.get<Health, PhysicsBody>(e.entity);

    health.get() -= e.damageAmount;

    if (e.impactForce.length() == 0.f) return;

    if (!body.ragdoll)
    {
        auto future = scene.actors.create();
        auto callback = scene.actors.emplace<TimedScript>(
            future,
            sf::seconds(0.5f),
            [&, entity = e.entity]
            {
                auto body2 = scene.actors.try_get<PhysicsBody>(entity);
                if (!body2) return;

                // Turn back original physics body behavior
                body2->ragdoll = false;
                body2->friction = ACTOR_FRICTION;
            });
    }

    // Turn the body into ragdoll
    body.forward += e.impactForce;
    body.ragdoll = true;
    body.friction = RAGDOLL_FRICTION;
}

void GameRulesEngine::update(const dgm::Time& time)
{
    if (scene.survivalGameDirector)
        scene.survivalGameDirector->update(time.getElapsed(), eventQueue);

    updateEntitiesWithInput(time);

    updateLifetimes(time);

    for (auto&& [entity, interval] : scene.actors.view<Interval>().each())
    {
        if (interval.timer > sf::Time::Zero)
        {
            interval.timer -= time.getElapsed();
        }
    }

    for (auto&& [entity, script] : scene.actors.view<TimedScript>().each())
    {
        if (script.timer > sf::Time::Zero)
            script.timer -= time.getElapsed();
        else
        {
            script.callback();
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
        }
    }

    updateHealth();

    updateTriggers(time);

    if (scene.hudMessage.displayTime > sf::Time::Zero)
        scene.hudMessage.displayTime -= time.getElapsed();
}

void GameRulesEngine::updateEntitiesWithInput(const dgm::Time& time)
{
    for (auto&& [entity, controller, body, lookDirection, weaponInventory] :
         scene.actors
             .view<EntityInput, PhysicsBody, LookDirection, WeaponInventory>()
             .each())
    {
        auto&& forwardImpulse = controller->getForward();
        if (body.ragdoll) forwardImpulse = sf::Vector2f {};
        body.forward += forwardImpulse * body.maxSpeed;

        if (forwardImpulse.length() == 0.f)
        {
            eventQueue.pushEvent<event::ActorStopped>(entity);
        }
        else
        {
            eventQueue.pushEvent<event::ActorMoved>(entity);
        }

        const auto direction = controller->getAimDirection();
        if (direction.length() > 0.f) lookDirection = LookDirection(direction);

        auto&& weapon = getActiveWeapon(weaponInventory);
        if (controller->isShootPressed()
            && weapon.timeTillFire <= sf::Time::Zero)
        {
            eventQueue.pushEvent<event::ActorStartedAttack>(entity);
        }
        else if (controller->isSwapWeaponsPressed())
        {
            weaponInventory.swapWeapon();
        }
        else if (controller->isInteractPressed() && scene.interactionTrigger)
        {
            if (scene.interactionTrigger->type
                == InteractionTriggerType::Workbench)
            {
                eventQueue.pushEvent<event::WorkbenchOpened>();
            }
        }

        // Reload all weapons at once
        for (auto&& w : weaponInventory.weapons)
        {
            w.timeTillFire -= time.getElapsed();
            if (w.timeTillFire < sf::Time::Zero)
                w.timeTillFire = sf::Time::Zero;
        }
    }
}

void GameRulesEngine::updateTriggers(const dgm::Time& time)
{
    scene.interactionTrigger.reset();

    for (auto&& [entity, inventory] :
         scene.actors.view<DelayedOnLeaveTriggerInventory>().each())
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

void GameRulesEngine::updateHealth()
{
    for (auto&& [entity, health] : scene.actors.view<Health>().each())
    {
        if (health.get() > 0) continue;

        auto skin = scene.actors.try_get<Skin>(entity);
        if (skin && skin->kind == EntityKind::Prop
            && skin->animation.getStateName() == "cactus-pot")
        {
            skin->animation.setState("cactus-pot-destroyed", "looping"_true);
            ActorBuilder::createParticleSystem(
                scene.actors,
                scene.actors.get<Collider>(entity).getPosition(),
                sf::Vector2f { 0.f, -1.f },
                ParticleSystemKind::CactusSpatter);
            scene.actors.remove<Health>(entity);
        }
        else
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
        }
    }
}

void GameRulesEngine::updateLifetimes(const dgm::Time& time)
{
    for (auto&& [entity, lifetime] : scene.actors.view<Lifetime>().each())
    {
        lifetime.get() -= time.getElapsed();
        if (lifetime.get() > sf::Time::Zero) continue;

        auto inventory = scene.actors.try_get<ProjectileInventory>(entity);
        if (inventory && inventory->traits & ProjectileTraits::Turret)
        {
            ActorBuilder::createTurret(
                scene.actors,
                scene.actors.get<Collider>(entity).getPosition(),
                atlas,
                *inventory);
        }

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

    if (skin->kind == inventory->originator) return;

    createDamageMarkerForProjectile(projectile, inventory);

    if (inventory->traits & ProjectileTraits::Passthru) return;

    eventQueue.pushEvent<event::ProjectileDestroyed>(projectile);
}

void GameRulesEngine::createDamageMarkerForProjectile(
    entt::entity projectile, ProjectileInventory* inventory)
{
    auto interval = scene.actors.try_get<Interval>(projectile);
    if (interval && interval->timer > sf::Time::Zero) return;

    const float scaleFactor =
        inventory->traits & ProjectileTraits::Explosive ? 2.f : 1.f;

    ActorBuilder::createDamageMarker(
        scene.actors,
        dgm::Math::toUnit(scene.actors.get<PhysicsBody>(projectile).forward),
        scene.actors.get<Collider>(projectile).getPosition(),
        (inventory->traits & ProjectileTraits::Explosive
             ? BASE_EXPLOSION_RADIUS
             : scene.actors.get<Collider>(projectile).getRadius())
            * scaleFactor,
        *inventory);

    if (interval)
    {
        interval->timer = interval->delay;
    }
}

void GameRulesEngine::handleDamageMarkerToActorCollision(
    entt::entity marker, entt::entity actor)
{
    auto inventory = scene.actors.try_get<DamageMarkerInventory>(marker);
    if (!inventory) return;

    auto [skin, health] = scene.actors.try_get<Skin, Health>(actor);
    if (!skin || !health || skin->kind == inventory->originator) return;

    eventQueue.pushEvent<event::ActorDamaged>(
        actor, inventory->damage, inventory->impactForceImpulse);
}

void GameRulesEngine::handleTriggerToActorCollision(
    entt::entity triggerIdx, entt::entity actorIdx)
{
    // Nobody except for the player can interact with stuff
    auto skin = scene.actors.try_get<Skin>(actorIdx);
    if (!skin || skin->kind != EntityKind::Player) return;

    if (auto&& inventory =
            scene.actors.try_get<DelayedOnLeaveTriggerInventory>(triggerIdx))
    {
        inventory->delay = BASE_DOOR_CLOSE_DELAY;
        auto&& targetSkin = scene.actors.get<Skin>(inventory->targetEntity);

        if (targetSkin.animation.getStateName() == DOOR_CLOSED_ANIMATION_STATE)
        {
            targetSkin.animation.setState(
                DOOR_OPENING_ANIMATION_STATE, "looping"_false);
        }
    }
    else if (
        auto&& inventory2 =
            scene.actors.try_get<InteractionTriggerInventory>(triggerIdx))
    {
        scene.interactionTrigger = *inventory2;
    }
}

sf::Vector2f GameRulesEngine::pickEnemySpawnPosition() const
{
    const auto playerPos =
        scene.actors.get<Collider>(scene.playerEntity).getPosition();

    auto filtered =
        scene.enemySpawns
        | std::views::filter(
            [&playerPos](const auto& spawn)
            { return (spawn - playerPos).length() > MIN_ENEMY_SPAWN_DISTANCE; })
        | uni::ranges::to<std::vector>();

    return filtered[rand() % filtered.size()];
}
