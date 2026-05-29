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
}

void GameRulesEngine::operator()(const event::ObjectDestroyed& e)
{
    const auto skin = scene.actors.try_get<Skin>(e.entity);
    if (skin && skin->kind == EntityKind::Npc)
    {
        ++scene.survivalSpawnerContext.enemiesKilledInCurrentWave;
        scene.status.score +=
            getScoreForEnemy(*skin) * scene.survivalSpawnerContext.wave;

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
    else if (skin && skin->kind == EntityKind::Player)
    {
        scene.status.finished = true;
    }
}

void GameRulesEngine::update(const dgm::Time& time)
{
    updateSpawner(time);

    updateEntitiesWithInput(time);

    updateLifetimes(time);

    for (auto&& [entity, interval] : scene.actors.view<Interval>().each())
    {
        if (interval.timer > sf::Time::Zero)
        {
            interval.timer -= time.getElapsed();
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
            weaponInventory.activeWeapon = 1 - weaponInventory.activeWeapon;
        }

        weapon.timeTillFire -= time.getElapsed();
        if (weapon.timeTillFire < sf::Time::Zero)
            weapon.timeTillFire = sf::Time::Zero;
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
        eventQueue.pushEvent<event::ObjectDestroyed>(entity);
    }
}

static SkinType getNpcToSpawn(int enemiesSpawnedInThisWave)
{
    if (enemiesSpawnedInThisWave % 2 == 0)
        return SkinType::Scuttlebug;
    else if (enemiesSpawnedInThisWave % 8 == 1)
        return SkinType::Bighead;
    else if (enemiesSpawnedInThisWave % 8 == 5)
        return SkinType::Beholder;
    return SkinType::ScuttlebugBlue;
}

void GameRulesEngine::updateSpawner(const dgm::Time& time)
{
    auto& context = scene.survivalSpawnerContext;

    // wave == -1 means Story mode, skip survival spawner logic
    if (context.wave == -1) return;

    if (context.state == SurvivalModeState::WaitingForNextWave)
    {
        context.timeTillNextWave -= time.getElapsed();
        if (context.timeTillNextWave <= sf::Time::Zero)
        {
            ++context.wave;
            context.enemiesSpawnedInCurrentWave = 0;
            context.enemiesKilledInCurrentWave = 0;
            context.enemiesInCurrentWave = context.wave * 25;
            context.state = SurvivalModeState::SpawningEnemies;
        }
    }
    else if (context.state == SurvivalModeState::SpawningEnemies)
    {
        context.timeTillNextSpawn -= time.getElapsed();
        if (context.timeTillNextSpawn <= sf::Time::Zero)
        {
            context.timeTillNextSpawn = SPAWNER_SPAWN_DELAY;
            ++context.enemiesSpawnedInCurrentWave;
            ActorBuilder::createNpc(
                scene.actors,
                pickEnemySpawnPosition(),
                getNpcToSpawn(context.enemiesSpawnedInCurrentWave),
                atlas);

            if (context.enemiesInCurrentWave
                == context.enemiesSpawnedInCurrentWave)
            {
                context.state = SurvivalModeState::WaitingForEnemiesToDie;
            }
        }
    }
    else if (context.state == SurvivalModeState::WaitingForEnemiesToDie)
    {
        if (context.enemiesKilledInCurrentWave == context.enemiesInCurrentWave)
        {
            context.state = SurvivalModeState::WaitingForNextWave;
            context.timeTillNextWave = sf::seconds(5.f);
            tryUnlockRandomModule();
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

void GameRulesEngine::tryUnlockRandomModule()
{
    constexpr std::array<WeaponModule, 9> ALL_NON_NONE_MODULES = {
        WeaponModule::SpreadBarrel_x4, WeaponModule::SpreadBarrel_x2,
        WeaponModule::CadenceBarrel,   WeaponModule::ExplosiveAmmo,
        WeaponModule::Ricochet,        WeaponModule::PassthruAmmo,
        WeaponModule::BigBullet,       WeaponModule::Spikes,
        WeaponModule::Splitter,
    };

    auto& unlocked = scene.loadout.unlockedModules;

    // Collect modules that are not yet unlocked
    std::vector<WeaponModule> locked;
    for (auto module : ALL_NON_NONE_MODULES)
    {
        if (!unlocked.contains(module)) locked.push_back(module);
    }

    if (locked.empty()) return; // All modules already unlocked

    const auto chosen = locked[rand() % locked.size()];
    unlocked.insert(chosen);
    postMessage("New weapon module unlocked!");
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
    if (!skin || !health) return;

    if (skin->kind == inventory->originator) return;

    health->get() -= inventory->damage;
    skin->animation.setState(HURT_ANIMATION_STATE, "looping"_false);
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

int GameRulesEngine::getScoreForEnemy(const Skin& skin) const
{
    if (skin.skinType == SkinType::Scuttlebug)
        return 1;
    else if (skin.skinType == SkinType::ScuttlebugBlue)
        return 2;
    else if (skin.skinType == SkinType::Bighead)
        return 4;
    else if (skin.skinType == SkinType::Beholder)
        return 8;
    return 0;
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
