#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/builders/WeaponBuilder.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/definitions/EnemyConstants.hpp"
#include "appstate/Game/definitions/NpcBlackboard.hpp"
#include "appstate/Game/definitions/PropConstants.hpp"
#include "appstate/Game/input/NpcInput.hpp"
#include "appstate/Game/input/PlayerInput.hpp"
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
    actors.emplace<Collider>(
        entity,
        dgm::Circle(spawnPosition, 8.f),
        ColliderOptions {
            .semighost = SEMIGHOST_PLAYER,
        });
    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody {
            .maxSpeed = BASE_PLAYER_SPEED,
            .friction = ACTOR_FRICTION,
        });
    actors.emplace<Skin>(
        entity,
        EntityKind::Player,
        SkinType::PlayerDefault,
        dgm::Animation(
            atlas.getSkinAnimationStates(SkinType::PlayerDefault),
            BASE_ANIMATION_FPS),
        sf::Vector2f { 0.f, -10.f });
    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
    actors.emplace<Health>(entity, BASE_PLAYER_HEALTH);
    actors.emplace<ZIndex>(entity, ZINDEX_COMMON);
    actors.emplace<WeaponInventory>(
        entity,
        0,
        loadout.weapons
            | uni::views::transform(
                [&](const WeaponConfig& config) -> Weapon
                {
                    return WeaponBuilder::createWeapon(
                        EntityKind::Player, config);
                })
            | uni::ranges::to<std::vector>());
    actors.emplace<EntityInput>(entity, std::make_unique<PlayerInput>(input));
    actors.emplace<BoundLightEmitter>(
        entity, BoundLightEmitter { sf::Color::Black, 10u });

    actors.get<Skin>(entity).animation.setState(
        IDLE_ANIMATION_STATE, "looping"_true);

    return entity;
}

entt::entity ActorBuilder::createNpc(
    entt::registry& actors,
    const sf::Vector2f& spawnPosition,
    const SkinType skin,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();
    assert(ENEMY_CONFIGS.contains(skin));
    const auto& config = ENEMY_CONFIGS.at(skin);

    actors.emplace<Collider>(
        entity,
        dgm::Circle(spawnPosition, config.colliderRadius),
        ColliderOptions {
            .semighost = config.semighostFlags,
        });

    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody {
            .maxSpeed = config.speed,
            .friction = ACTOR_FRICTION,
            .useAltMesh = true,
            .canFall = config.canFall,
            .pinned = false,
        });

    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });

    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = EntityKind::Npc,
            .skinType = skin,
            .animation = dgm::Animation(
                atlas.getSkinAnimationStates(skin), BASE_ANIMATION_FPS),
            .spriteOriginOffsetFromCollider =
                config.spriteOriginOffsetFromCollider,
        });

    actors.emplace<Health>(entity, config.health);

    if (config.isRanged)
    {
        actors.emplace<WeaponInventory>(
            entity,
            0,
            std::vector<Weapon> {
                WeaponBuilder::createRangedWeapon(
                    EntityKind::Npc, SkinType::Fireball, config.damage),
            });
    }
    else
    {
        actors.emplace<WeaponInventory>(
            entity,
            0,
            std::vector<Weapon> { WeaponBuilder::createMeleeWeapon(
                EntityKind::Npc, config.damage) });
    }

    auto input = std::make_unique<NpcInput>();
    auto underlyingInput = input.get();
    actors.emplace<EntityInput>(entity, std::move(input));
    actors.emplace<NpcBlackboard>(
        entity,
        NpcBlackboard {
            .ownerEntity = entity,
            .input = dynamic_cast<NpcInput&>(*underlyingInput),
            .kind = config.npcKind,
        });
    actors.emplace<ZIndex>(entity, ZINDEX_COMMON);

    actors.get<Skin>(entity).animation.setState(
        IDLE_ANIMATION_STATE, "looping"_true);

    return entity;
}

entt::entity ActorBuilder::createProp(
    entt::registry& actors,
    const sf::Vector2f& origin,
    size_t propId,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();
    assert(propId < PROP_DEFINITIONS.size());
    const auto& propDef = PROP_DEFINITIONS[propId];

    auto&& collider = propDef.getCollider(origin);
    actors.emplace<Collider>(entity, collider);

    if (propDef.isSolid) actors.emplace<PhysicsBody>(entity);
    actors.emplace<Skin>(
        entity,
        EntityKind::Prop,
        SkinType::Prop,
        dgm::Animation(
            atlas.getSkinAnimationStates(SkinType::Prop), BASE_ANIMATION_FPS),
        propDef.spriteOffset);

    actors.get<Skin>(entity).animation.setState(
        propDef.animationStateName, "looping"_true);
    actors.emplace<ZIndex>(
        entity, propDef.isSolid ? ZINDEX_COMMON : ZINDEX_FLOOR_DECOR);

    if (propDef.boundLightEmitter.has_value())
    {
        actors.emplace<BoundLightEmitter>(
            entity, propDef.boundLightEmitter.value());
    }

    if (propDef.animationStateName == "workbench")
    {
        const auto triggerHalf =
            sf::Vector2f { collider.getRadius(), collider.getRadius() };
        auto trigger = actors.create();
        actors.emplace<Collider>(
            trigger,
            dgm::Rect(collider.getPosition() - triggerHalf, triggerHalf * 2.f),
            ColliderOptions {
                .reportActorCollisions = true,
                .nonblocking = true,
            });
        actors.emplace<InteractionTriggerInventory>(
            trigger, InteractionTriggerType::Workbench);
    }

    if (propDef.health)
    {
        actors.emplace<Health>(entity, *propDef.health);
    }

    return entity;
}

void ActorBuilder::destroyProp(entt::registry& actors, entt::entity entity)
{
    auto& skin = actors.get<Skin>(entity);
    if (skin.animation.getStateName() == "cactus-pot")
    {
        actors.remove<Health>(entity);

        auto& collider = actors.get<Collider>(entity);
        collider.options.semighost = SEMIGHOST_PROJECTILE;
        skin.animation.setState("cactus-pot-destroyed", "looping"_true);

        createParticleSystem(
            actors,
            collider.getPosition(),
            sf::Vector2f { 1.f, 0.f },
            ParticleSystemKind::CactusSpatter);
    }
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
    const float speedVariance = static_cast<float>(rand() % 200 - 100) / 100.f
                                * weapon.projectileSpeedVariance;

    const auto hasShrapnelsTrait =
        weapon.defaultProjectileInventory.traits & ProjectileTraits::Shrapnels;
    const auto hasTurretTrait =
        weapon.defaultProjectileInventory.traits & ProjectileTraits::Turret;
    const auto friction = hasShrapnelsTrait ? 0.02f
                          : hasTurretTrait  ? 0.1f
                                            : 0.f;
    const auto reportMeshCollisions = !hasShrapnelsTrait && !hasTurretTrait;
    // const auto nonblocking =
    //     !(weapon.defaultProjectileInventory.traits &
    //     ProjectileTraits::Turret);

    auto entity = actors.create();
    actors.emplace<Collider>(
        entity,
        dgm::Circle(origin, 3.f * sizeFactor),
        ColliderOptions {
            .reportMeshCollisions = reportMeshCollisions,
            .reportActorCollisions = true,
            .nonblocking = true,
            .semighost = hasShrapnelsTrait ? 0 : SEMIGHOST_PROJECTILE,
        });
    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody {
            .forward = direction * (weapon.projectileSpeed + speedVariance),
            .bounciness = weapon.defaultProjectileInventory.traits
                                  & ProjectileTraits::Bouncy
                              ? 0.8f
                              : 0.f,
            .friction = friction,
            .useAltMesh = true,
            .canFall = hasShrapnelsTrait,
        });
    actors.emplace<Lifetime>(entity, weapon.projectileLifetime);
    actors.emplace<ZIndex>(
        entity, hasShrapnelsTrait ? ZINDEX_FLOOR_DECOR : ZINDEX_PROJECTILES);

    auto animation = dgm::Animation(
        atlas.getSkinAnimationStates(weapon.projectileSkin),
        BASE_ANIMATION_FPS);
    animation.setState(IDLE_ANIMATION_STATE, "looping"_true);
    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = EntityKind::Projectile,
            .skinType = weapon.projectileSkin,
            .animation = std::move(animation),
            .scale = sizeFactor,
        });

    actors.emplace<ProjectileInventory>(
        entity, weapon.defaultProjectileInventory);

    if (weapon.defaultProjectileInventory.traits & ProjectileTraits::Passthru)
    {
        actors.emplace<Interval>(
            entity, Interval { .delay = sf::milliseconds(30) });
    }
    return entity;
}

entt::entity ActorBuilder::createTurret(
    entt::registry& actors,
    const sf::Vector2f& origin,
    const GameTextureAtlas& atlas,
    const ProjectileInventory& inventory)
{
    auto entity = actors.create();

    actors.emplace<Collider>(
        entity,
        dgm::Circle(origin, 8.f),
        ColliderOptions {
            .semighost = SEMIGHOST_PLAYER | SEMIGHOST_NPC,
        });

    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody {
            .maxSpeed = 0.0f,
            .friction = 0.5f,
            .useAltMesh = true,
            .canFall = true,
        });

    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });

    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = EntityKind::Player,
            .skinType = SkinType::Turret,
            .animation = dgm::Animation(
                atlas.getSkinAnimationStates(SkinType::Turret),
                BASE_ANIMATION_FPS),
        });

    actors.emplace<Health>(entity, BASE_TURRET_HEALTH);
    actors.emplace<Lifetime>(entity, EXTENDED_PROJECTILE_LIFETIME);

    auto weaponConfig = WeaponConfig {};
    for (auto&& idx : uni::views::iota(
             0u,
             std::min(
                 weaponConfig.modules.size(), inventory.spawnerDef.size())))
    {
        weaponConfig.modules[idx] = inventory.spawnerDef[idx];
    }

    actors.emplace<WeaponInventory>(
        entity,
        0,
        std::vector<Weapon> {
            WeaponBuilder::createWeapon(EntityKind::Player, weaponConfig),
        });

    auto input = std::make_unique<NpcInput>();
    auto underlyingInput = input.get();
    actors.emplace<EntityInput>(entity, std::move(input));
    actors.emplace<NpcBlackboard>(
        entity,
        NpcBlackboard {
            .ownerEntity = entity,
            .input = dynamic_cast<NpcInput&>(*underlyingInput),
            .kind = NpcKind::Turret,
        });
    actors.emplace<ZIndex>(entity, ZINDEX_COMMON);

    actors.get<Skin>(entity).animation.setState("spawn", "looping"_false);

    return entity;
}

void ActorBuilder::shatterProjectile(
    entt::registry& actors, entt::entity projectile)
{
    auto body = actors.get<PhysicsBody>(projectile);
    auto skin = actors.get<Skin>(projectile);
    skin.scale *= 0.5f;
    auto inventory = actors.get<ProjectileInventory>(projectile);
    // TODO: somehow substract the trait for splitting
    inventory.damage /= 2;
    inventory.traits = inventory.traits - ProjectileTraits::SplitOnHit;

    for (auto direction = sf::degrees(45); direction < sf::degrees(360);
         direction += sf::degrees(90))
    {
        auto entity = actors.create();
        actors.emplace<Collider>(entity, actors.get<Collider>(projectile));

        body.forward = body.forward.rotatedBy(sf::degrees(90));
        actors.emplace<PhysicsBody>(entity, body);

        actors.emplace<Lifetime>(entity, actors.get<Lifetime>(projectile));
        actors.emplace<ZIndex>(entity, actors.get<ZIndex>(projectile));
        actors.emplace<Skin>(entity, skin);
        actors.emplace<ProjectileInventory>(entity, inventory);

        if (auto interval = actors.try_get<Interval>(projectile))
        {
            actors.emplace<Interval>(entity, *interval);
        }
    }
}

entt::entity ActorBuilder::createDamageMarker(
    entt::registry& actors,
    const sf::Vector2f& projectileDirection,
    const sf::Vector2f& origin,
    const float radius,
    const ProjectileInventory& inventory)
{
    const auto pushForce =
        (inventory.traits & ProjectileTraits::Push ? BASE_PROJECTILE_PUSH_IMPACT
                                                   : 0.f)
        * (inventory.traits & ProjectileTraits::Big ? 2.f : 1.f);

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
            .impactForceImpulse = projectileDirection * pushForce,
        });
    actors.emplace<Lifetime>(entity, sf::Time::Zero);

    return entity;
}

entt::entity ActorBuilder::createEffect(
    entt::registry& actors,
    const sf::Vector2f& origin,
    const EffectType type,
    const GameTextureAtlas& atlas,
    const float scale)
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
                .kind = EntityKind::Effect,
                .skinType = SkinType::BigBullet,
                .animation = dgm::Animation(
                    atlas.getSkinAnimationStates(SkinType::BigBullet),
                    BASE_ANIMATION_FPS),
            });
    }
    else if (type == EffectType::Explosion)
    {
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = EntityKind::Effect,
                .skinType = SkinType::Explosion,
                .animation = dgm::Animation(
                    atlas.getSkinAnimationStates(SkinType::Explosion),
                    BASE_ANIMATION_FPS),
                .scale = scale,
            });
    }
    else if (type == EffectType::FireballDecay)
    {
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = EntityKind::Effect,
                .skinType = SkinType::Fireball,
                .animation = dgm::Animation(
                    atlas.getSkinAnimationStates(SkinType::Fireball),
                    BASE_ANIMATION_FPS),
                .scale = scale,
            });
    }
    else
    {
        assert(false && "Invalid effect type");
    }

    actors.emplace<ZIndex>(entity, ZINDEX_PROJECTILES);
    auto&& animation = actors.get<Skin>(entity).animation;
    animation.setState(DEATH_ANIMATION_STATE, "looping"_false);

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
    actors.emplace<ZIndex>(entity, 1);
    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = EntityKind::Door,
            .skinType = SkinType::DoorHorizontal,
            .animation = dgm::Animation(
                atlas.getSkinAnimationStates(SkinType::DoorHorizontal),
                BASE_ANIMATION_FPS),
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
    actors.emplace<DelayedOnLeaveTriggerInventory>(
        trigger, DelayedOnLeaveTriggerInventory { .targetEntity = entity });

    return entity;
}

struct [[nodiscard]] ParticleSystemDefinition final
{
    sf::Time emissionInterval;
    sf::Time lifetime;
    int particlesToEmit;
    sf::Angle directionVariance;
    std::vector<sf::Color> colors;
    float maxParticleSize;
    float speed;
    float speedVariance;
};

const std::map<ParticleSystemKind, ParticleSystemDefinition>
    PARTICLE_SYSTEM_DEF = {
        {
            ParticleSystemKind::BloodSpatter,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.5f),
                .particlesToEmit = 16,
                .directionVariance = sf::degrees(100),
                .colors = { COLOR_RED, COLOR_DARK_RED },
                .maxParticleSize = 4.f,
                .speed = 30.f,
                .speedVariance = 10.f,
            },
        },
        {
            ParticleSystemKind::ProjectileImpact,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.25f),
                .particlesToEmit = 10,
                .directionVariance = sf::degrees(15),
                .colors = { COLOR_ORANGE, COLOR_YELLOW },
                .maxParticleSize = 1.f,
                .speed = 60.f,
                .speedVariance = 10.f,
            },
        },
        {
            ParticleSystemKind::CactusSpatter,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.5f),
                .particlesToEmit = 16,
                .directionVariance = sf::degrees(100),
                .colors = { COLOR_GREEN, COLOR_DARK_GREEN },
                .maxParticleSize = 4.f,
                .speed = 30.f,
                .speedVariance = 10.f,
            },
        },
        {
            ParticleSystemKind::Explosion,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.35f),
                .particlesToEmit = 48,
                .directionVariance = sf::degrees(360),
                .colors = { COLOR_DARK_GREY,
                            COLOR_ORANGE,
                            COLOR_YELLOW,
                            COLOR_WHITE },
                .maxParticleSize = 7.f,
                .speed = 60.f,
                .speedVariance = 30.f,
            },
        },
        {
            ParticleSystemKind::HugeExplosion,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.5f),
                .particlesToEmit = 48,
                .directionVariance = sf::degrees(360),
                .colors = { COLOR_DARK_GREY,
                            COLOR_ORANGE,
                            COLOR_YELLOW,
                            COLOR_WHITE },
                .maxParticleSize = 10.f,
                .speed = 60.f,
                .speedVariance = 30.f,
            },
        },
        {
            ParticleSystemKind::FireballExplosion,
            ParticleSystemDefinition {
                .emissionInterval = sf::Time::Zero,
                .lifetime = sf::seconds(0.4f),
                .particlesToEmit = 24,
                .directionVariance = sf::degrees(360),
                .colors = { COLOR_RED, COLOR_ORANGE },
                .maxParticleSize = 6.f,
                .speed = 60.f,
                .speedVariance = 0.f,
            },
        },
    };

entt::entity ActorBuilder::createParticleSystem(
    entt::registry& actors,
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    ParticleSystemKind kind)
{
    auto entity = actors.create();

    const auto& def = PARTICLE_SYSTEM_DEF.at(kind);

    actors.emplace<ParticleEmitter>(
        entity,
        ParticleEmitter {
            .emissionInterval = def.emissionInterval,
            .emissionTimer = sf::seconds(0.f),
            .particlesToEmit = def.particlesToEmit,
            .position = origin,
            .direction = direction,
            .directionVariance = def.directionVariance,
            .colors = def.colors,
            .maxProjectileSize = def.maxParticleSize,
            .speed = def.speed,
            .speedVariance = def.speedVariance,

        });

    actors.emplace<ParticleSystem>(entity);
    actors.emplace<Lifetime>(entity, def.lifetime);

    return entity;
}
