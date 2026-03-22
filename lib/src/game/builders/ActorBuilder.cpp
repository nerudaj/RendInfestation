#include "game/builders/ActorBuilder.hpp"
#include "game/builders/WeaponBuilder.hpp"
#include "game/definitions/Constants.hpp"
#include "game/definitions/NpcBlackboard.hpp"
#include "game/input/NpcInput.hpp"
#include "game/input/PlayerInput.hpp"
#include "types/SemanticTypes.hpp"
#include <memory>

entt::entity ActorBuilder::createPlayer(
    entt::registry& actors,
    const sf::Vector2f& spawnPosition,
    const GameTextureAtlas& atlas,
    Input& input,
    const WeaponLoadout& loadout)
{
    auto entity = actors.create();
    actors.emplace<Collider>(entity, dgm::Circle(spawnPosition, 8.f));
    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody { .maxSpeed = BASE_PLAYER_SPEED, .friction = 0.8f });
    actors.emplace<Skin>(
        entity,
        ActorKind::Player,
        SkinType::PlayerDefault,
        dgm::Animation(atlas.atlas.getAnimationStates(atlas.playerLocation), 8),
        sf::Vector2f { 0.f, -10.f });
    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
    actors.emplace<Health>(entity, 10000);
    actors.emplace<WeaponInventory>(
        entity,
        0,
        std::vector<Weapon> { WeaponBuilder::createWeapon(
                                  ActorKind::Player,
                                  { loadout.weapon1Modules[0],
                                    loadout.weapon1Modules[1],
                                    loadout.weapon1Modules[2] }),
                              WeaponBuilder::createWeapon(
                                  ActorKind::Player,
                                  { loadout.weapon2Modules[0],
                                    loadout.weapon2Modules[1],
                                    loadout.weapon2Modules[2] }) });
    actors.emplace<EntityInput>(entity, std::make_unique<PlayerInput>(input));
    actors.emplace<BoundLightEmitter>(
        entity, BoundLightEmitter { COLOR_WHITE, 10u });

    actors.get<Skin>(entity).animation.setState("idle-front", "looping"_true);

    return entity;
}

entt::entity ActorBuilder::createNpc(
    entt::registry& actors,
    const sf::Vector2f& spawnPosition,
    const SkinType skin,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();

    actors.emplace<Collider>(entity, dgm::Circle(spawnPosition, 8.f));
    actors.emplace<PhysicsBody>(
        entity, PhysicsBody { .maxSpeed = BASE_ENEMY_SPEED, .friction = 0.8f });
    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = ActorKind::Npc,
            .skinType = skin,
            .animation = dgm::Animation(atlas.getSkinAnimationStates(skin), 8),
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
        });
    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
    actors.emplace<Health>(entity, skin == SkinType::Bighead ? 100 : 50);
    actors.emplace<WeaponInventory>(
        entity,
        0,
        std::vector<Weapon> { WeaponBuilder::createMeleeWeapon(
            ActorKind::Npc,
            skin == SkinType::Bighead ? BASE_MELEE_DAMAGE
                                      : BASE_MELEE_DAMAGE / 2) });
    actors.emplace<EntityInput>(entity, std::make_unique<NpcInput>());
    actors.emplace<NpcBlackboard>(entity);

    actors.get<Skin>(entity).animation.setState("walk-front", "looping"_true);

    return entity;
}

static Collider getPropCollider(const sf::Vector2f& origin, const size_t propId)
{
    if (propId == 0 || propId == 1)
    {
        return Collider { dgm::Circle(
            { origin.x + 32.f, origin.y - 16.f }, 13.f) };
    }
    else if (propId == 2)
    {
        return Collider { dgm::Rect(
            { origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }) };
    }
    else if (propId == 4 || propId == 5)
        return Collider {
            dgm::Rect({ origin.x, origin.y - 64.f }, { 32.f, 32.f }),
            ColliderOptions { .nonblocking = true, .disabled = true }
        };
    else if (propId == 6)
        return Collider {
            dgm::Rect({ origin.x, origin.y - 64.f }, { 20.f, 14.f }),
            ColliderOptions { .nonblocking = true, .disabled = true }
        };
    else if (propId == 7)
        return Collider {
            dgm::Rect({ origin.x, origin.y - 64.f }, { 24.f, 14.f }),
            ColliderOptions { .nonblocking = true, .disabled = true }
        };

    return Collider { dgm::Rect(
        { origin.x, origin.y - 64.f }, { 64.f, 64.f }) };
}

static sf::Vector2f getPropSpriteOffset(const size_t propId)
{
    if (propId == 0 || propId == 1)
    {
        return { 0.f, -16.f };
    }
    else if (propId == 2)
    {
        return { 0.f, 0.f };
    }
    else if (propId == 4 || propId == 5)
        return { 16.f, 16.f };
    else if (propId == 6)
        return { 22.f, 25.f };
    else if (propId == 7)
        return { 20.f, 25.f };

    return { 0.f, 0.f };
}

static bool isPropPassable(const size_t propId)
{
    return propId == 4 || propId == 5 || propId == 6 || propId == 7;
}

static bool isPropLabtune(const size_t propId)
{
    return propId == 0 || propId == 1;
}

entt::entity ActorBuilder::createProp(
    entt::registry& actors,
    const sf::Vector2f& origin,
    size_t propId,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();
    actors.emplace<Collider>(entity, getPropCollider(origin, propId));

    if (!isPropPassable(propId)) actors.emplace<PhysicsBody>(entity);
    actors.emplace<Skin>(
        entity,
        ActorKind::Prop,
        SkinType::Prop,
        dgm::Animation(atlas.atlas.getAnimationStates(atlas.propsLocation)),
        getPropSpriteOffset(propId));

    auto stateName = [](size_t id)
    {
        if (id == 0)
            return "labtube-full";
        else if (id == 1)
            return "labtube";
        else if (id == 2)
            return "small-table";
        else if (id == 3)
            return "cantina-table";
        else if (id == 4)
            return "green-carcass";
        else if (id == 5)
            return "blue-carcass";
        else if (id == 6)
            return "pc";
        else if (id == 7)
            return "blood-puddle-a";
    };

    actors.get<Skin>(entity).animation.setState(
        stateName(propId), "looping"_true);

    if (isPropLabtune(propId))
    {
        actors.emplace<BoundLightEmitter>(
            entity, BoundLightEmitter { COLOR_GREEN, 7 });
    }

    return entity;
}

entt::entity ActorBuilder::createProjectile(
    entt::registry& actors,
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    const GameTextureAtlas& atlas,
    const Weapon& weapon)
{
    const float sizeFactor =
        weapon.defaultProjectileInventory.traits & ProjectileTraits::Big ? 2.f
                                                                         : 1.f;

    auto entity = actors.create();
    actors.emplace<Collider>(
        entity,
        dgm::Circle(origin, 3.f * sizeFactor),
        "reportMeshCollisions"_true,
        "reportActorCollisions"_true,
        "nonblocking"_true);

    const float speedVariance = static_cast<float>(rand() % 200 - 100) / 100.f
                                * weapon.projectileSpeedVariance;

    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody {
            .forward = direction * (weapon.projectileSpeed + speedVariance),
            .bounciness = 0.8f,
            .friction = weapon.defaultProjectileInventory.traits
                                & ProjectileTraits::Shrapnels
                            ? 0.02f
                            : 0.f,
            .useAltMesh = true,
        });
    actors.emplace<Lifetime>(entity, weapon.projectileLifetime);

    auto animation =
        dgm::Animation(atlas.getSkinAnimationStates(weapon.projectileSkin), 8);
    animation.setState("idle", "looping"_true);
    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = ActorKind::Projectile,
            .skinType = weapon.projectileSkin,
            .animation = std::move(animation),
            .scale = sizeFactor,
        });

    actors.emplace<ProjectileInventory>(
        entity, weapon.defaultProjectileInventory);

    if (weapon.defaultProjectileInventory.traits & ProjectileTraits::Passthru)
    {
        actors.emplace<Interval>(
            entity, Interval { .delay = sf::seconds(0.1f) });
    }

    /*if (weapon.projectileSkin == SkinType::SmallBullet)
    {
        actors.emplace<BoundLightEmitter>(
            entity, BoundLightEmitter { COLOR_YELLOW, 10 });
    }
    else if (weapon.projectileSkin == SkinType::BigBullet)
    {
        actors.emplace<BoundLightEmitter>(
            entity, BoundLightEmitter { COLOR_YELLOW, 10 });
    }
    else if (weapon.projectileSkin == SkinType::Hyperbeam)
    {
        actors.emplace<BoundLightEmitter>(
            entity, BoundLightEmitter { COLOR_GREEN, 10 });
    }*/

    return entity;
}

entt::entity ActorBuilder::createDamageMarker(
    entt::registry& actors,
    const sf::Vector2f& origin,
    const float radius,
    const ProjectileInventory& inventory)
{
    auto entity = actors.create();
    actors.emplace<Collider>(
        entity,
        dgm::Circle(origin, radius),
        ColliderOptions {
            .reportActorCollisions = true,
            .nonblocking = true,
        });
    actors.emplace<DamageMarkerInventory>(
        entity,
        DamageMarkerInventory {
            .originator = inventory.originator,
            .damage = inventory.damage,
        });
    actors.emplace<Lifetime>(entity, sf::Time::Zero);

    return entity;
}

entt::entity ActorBuilder::createEffect(
    entt::registry& actors,
    const sf::Vector2f& origin,
    const EffectType type,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();

    actors.emplace<Collider>(
        entity,
        dgm::Circle(origin, 1.f),
        ColliderOptions {
            .disabled = true,
        });

    if (type == EffectType::BulletDeath)
    {
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = ActorKind::Effect,
                .skinType = SkinType::BigBullet,
                .animation = dgm::Animation(
                    atlas.atlas.getAnimationStates(atlas.bulletLocation),
                    EFFECT_FPS),
            });
    }
    else if (type == EffectType::Explosion)
    {
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = ActorKind::Effect,
                .skinType = SkinType::Explosion,
                .animation = dgm::Animation(
                    atlas.atlas.getAnimationStates(atlas.explosionLocation),
                    EFFECT_FPS),
            });
    }
    else
    {
        assert(false && "Invalid effect type");
    }

    auto&& animation = actors.get<Skin>(entity).animation;
    animation.setState("death", "looping"_false);
    /*    actors.get<Lifetime>(entity) = sf::seconds(
            static_cast<float>(animation.getCurrentStateFrameCount()) /
       EFFECT_FPS);*/

    return entity;
}

entt::entity ActorBuilder::createDoor(
    entt::registry& actors,
    const sf::Vector2f& position,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();
    actors.emplace<Collider>(entity, dgm::Rect(position, { 64.f, 42.f }));
    actors.emplace<PhysicsBody>(entity);
    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = ActorKind::Door,
            .skinType = SkinType::DoorHorizontal,
            .animation = dgm::Animation(
                atlas.atlas.getAnimationStates(atlas.doorHorizontalLocation),
                EFFECT_FPS),
        });

    actors.get<Skin>(entity).animation.setState(
        DOOR_CLOSED_ANIMATION_STATE, "looping"_true);

    // trigger
    auto trigger = actors.create();
    actors.emplace<Collider>(
        trigger,
        dgm::Rect(position - sf::Vector2f { 0.f, 27.f }, { 64.f, 96.f }),
        ColliderOptions {
            .reportActorCollisions = true,
            .nonblocking = true,
        });
    actors.emplace<TriggerInventory>(
        trigger, TriggerInventory { .targetEntity = entity });

    return entity;
}
