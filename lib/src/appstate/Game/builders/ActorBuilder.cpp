#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/builders/WeaponBuilder.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/definitions/NpcBlackboard.hpp"
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
    actors.emplace<Collider>(entity, dgm::Circle(spawnPosition, 8.f));
    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody { .maxSpeed = BASE_PLAYER_SPEED, .friction = 0.8f });
    actors.emplace<Skin>(
        entity,
        EntityKind::Player,
        SkinType::PlayerDefault,
        dgm::Animation(
            atlas.getSkinAnimationStates(SkinType::PlayerDefault),
            BASE_ANIMATION_FPS),
        sf::Vector2f { 0.f, -10.f });
    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
    actors.emplace<Health>(entity, 100);
    actors.emplace<ZIndex>(entity, ZINDEX_COMMON);
    actors.emplace<WeaponInventory>(
        entity,
        0,
        std::vector<Weapon> { WeaponBuilder::createWeapon(
                                  EntityKind::Player,
                                  { loadout.weapon1Modules[0],
                                    loadout.weapon1Modules[1],
                                    loadout.weapon1Modules[2] }),
                              WeaponBuilder::createWeapon(
                                  EntityKind::Player,
                                  { loadout.weapon2Modules[0],
                                    loadout.weapon2Modules[1],
                                    loadout.weapon2Modules[2] }) });
    actors.emplace<EntityInput>(entity, std::make_unique<PlayerInput>(input));
    actors.emplace<BoundLightEmitter>(
        entity, BoundLightEmitter { COLOR_WHITE, 10u });

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

    if (skin == SkinType::Scuttlebug)
    {
        actors.emplace<Collider>(
            entity,
            dgm::Circle(spawnPosition, 8.f),
            ColliderOptions { .semighost = true });
        actors.emplace<PhysicsBody>(
            entity,
            PhysicsBody { .maxSpeed = BASE_ENEMY_SPEED * 1.2f,
                          .friction = 0.8f });
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = EntityKind::Npc,
                .skinType = skin,
                .animation = dgm::Animation(
                    atlas.getSkinAnimationStates(skin), BASE_ANIMATION_FPS),
                .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            });
        actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
        actors.emplace<Health>(entity, 50);

        actors.emplace<WeaponInventory>(
            entity,
            0,
            std::vector<Weapon> { WeaponBuilder::createMeleeWeapon(
                EntityKind::Npc, BASE_MELEE_DAMAGE / 2) });
    }
    else if (skin == SkinType::ScuttlebugBlue)
    {
        actors.emplace<Collider>(
            entity,
            dgm::Circle(spawnPosition, 8.f),
            ColliderOptions { .semighost = true });
        actors.emplace<PhysicsBody>(
            entity,
            PhysicsBody { .maxSpeed = BASE_ENEMY_SPEED * 1.5f,
                          .friction = 0.8f });
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = EntityKind::Npc,
                .skinType = skin,
                .animation = dgm::Animation(
                    atlas.getSkinAnimationStates(skin), BASE_ANIMATION_FPS),
                .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            });
        actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
        actors.emplace<Health>(entity, 65);

        actors.emplace<WeaponInventory>(
            entity,
            0,
            std::vector<Weapon> { WeaponBuilder::createMeleeWeapon(
                EntityKind::Npc, BASE_MELEE_DAMAGE / 2) });
    }
    else if (skin == SkinType::Bighead)
    {
        actors.emplace<Collider>(
            entity,
            dgm::Circle(spawnPosition, 8.f),
            ColliderOptions { .semighost = true });
        actors.emplace<PhysicsBody>(
            entity,
            PhysicsBody { .maxSpeed = BASE_ENEMY_SPEED, .friction = 0.8f });
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = EntityKind::Npc,
                .skinType = skin,
                .animation = dgm::Animation(
                    atlas.getSkinAnimationStates(skin), BASE_ANIMATION_FPS),
                .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            });
        actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
        actors.emplace<Health>(entity, 100);

        actors.emplace<WeaponInventory>(
            entity,
            0,
            std::vector<Weapon> { WeaponBuilder::createMeleeWeapon(
                EntityKind::Npc, BASE_MELEE_DAMAGE) });
    }
    else if (skin == SkinType::Beholder)
    {
        actors.emplace<Collider>(
            entity,
            dgm::Circle(spawnPosition, 8.f),
            ColliderOptions { .semighost = true });
        actors.emplace<PhysicsBody>(
            entity,
            PhysicsBody { .maxSpeed = BASE_ENEMY_SPEED / 2.f,
                          .friction = 0.8f });
        actors.emplace<Skin>(
            entity,
            Skin {
                .kind = EntityKind::Npc,
                .skinType = skin,
                .animation = dgm::Animation(
                    atlas.getSkinAnimationStates(skin), BASE_ANIMATION_FPS),
                .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            });
        actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
        actors.emplace<Health>(entity, 120);

        actors.emplace<WeaponInventory>(
            entity,
            0,
            std::vector<Weapon> {
                WeaponBuilder::createRangedWeapon(
                    EntityKind::Npc,
                    SkinType::PinkFireball,
                    BASE_RANGED_DAMAGE),
            });
    }
    else
    {
        throw std::runtime_error(uni::format(
            "Invalid skin type {} for NPC", std::to_underlying(skin)));
    }

    auto input = std::make_unique<NpcInput>();
    auto underlyingInput = input.get();
    actors.emplace<EntityInput>(entity, std::move(input));
    actors.emplace<NpcBlackboard>(
        entity,
        NpcBlackboard {
            .ownerEntity = entity,
            .input = dynamic_cast<NpcInput&>(*underlyingInput),
            .kind =
                skin == SkinType::Beholder ? NpcKind::Ranged : NpcKind::Melee,
        });
    actors.emplace<ZIndex>(entity, ZINDEX_COMMON);

    actors.get<Skin>(entity).animation.setState(
        IDLE_ANIMATION_STATE, "looping"_true);

    return entity;
}

struct [[nodiscard]] Prop final
{
    std::string animationStateName;
    std::function<Collider(const sf::Vector2f&)> getCollider;
    sf::Vector2f spriteOffset = { 0.f, 0.f };
    bool isSolid = true;
    std::optional<BoundLightEmitter> boundLightEmitter = std::nullopt;
};

const std::array<Prop, 13u> PROP_DEFINITIONS = {
    Prop {
        .animationStateName = "labtube-full",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Circle({ origin.x + 32.f, origin.y - 16.f }, 13.f),
            };
        },
        .spriteOffset = { 0.f, -16.f },
        .boundLightEmitter = BoundLightEmitter { COLOR_GREEN, 7 },
    },
    Prop {
        .animationStateName = "labtube",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Circle({ origin.x + 32.f, origin.y - 16.f }, 13.f),
            };
        },
        .spriteOffset = { 0.f, -16.f },
        .boundLightEmitter = BoundLightEmitter { COLOR_GREEN, 7 },
    },
    Prop {
        .animationStateName = "small-table",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }) };
        },
    },
    Prop {
        .animationStateName = "cantina-table",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 64.f }, { 64.f, 56.f }) };
        },
        .spriteOffset = { 0.f, -4.f },
    },
    Prop {
        .animationStateName = "green-carcass",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 32.f, 32.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 16.f, 16.f },
        .isSolid = false,
    },
    Prop {
        .animationStateName = "blue-carcass",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 32.f, 32.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 16.f, 16.f },
        .isSolid = false,
    },
    Prop {
        .animationStateName = "blood-puddle-a",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 24.f, 14.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 22.f, 25.f },
        .isSolid = false,
    },
    Prop {
        .animationStateName = "pc",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 20.f, 14.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 22.f, 25.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "pc-active",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 20.f, 14.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 22.f, 25.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "cactus-pot",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 56.f }, { 15.f, 10.f }) };
        },
        .spriteOffset = { 25.f, 18.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "cactus-pot-destroyed",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 56.f }, { 15.f, 10.f }) };
        },
        .spriteOffset = { 25.f, 18.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "crate",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 64.f }, { 11.f, 10.f }) };
        },
        .spriteOffset = { 26.f, 23.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "workbench",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }) };
        },
    },
};

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
    actors.emplace<ZIndex>(entity, ZINDEX_PROJECTILES);

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
    std::array<sf::Color, 2> colors;
    float maxProjectileSize;
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
                .maxProjectileSize = 4.f,
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
                .maxProjectileSize = 1.f,
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
                .maxProjectileSize = 4.f,
                .speed = 30.f,
                .speedVariance = 10.f,
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
            .maxProjectileSize = def.maxProjectileSize,
            .speed = def.speed,
            .speedVariance = def.speedVariance,

        });

    actors.emplace<ParticleSystem>(entity);
    actors.emplace<Lifetime>(entity, def.lifetime);

    return entity;
}
