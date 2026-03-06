#include "game/builders/ActorBuilder.hpp"
#include "game/builders/WeaponBuilder.hpp"
#include "game/definitions/Constants.hpp"
#include "game/input/NpcInput.hpp"
#include "game/input/PlayerInput.hpp"
#include "types/SemanticTypes.hpp"
#include <memory>

entt::entity ActorBuilder::createPlayer(
    entt::registry& actors,
    const sf::Vector2f& spawnPosition,
    const GameTextureAtlas& atlas,
    Input& input)
{
    auto entity = actors.create();
    actors.emplace<Collider>(entity, dgm::Circle(spawnPosition, 8.f));
    actors.emplace<PhysicsBody>(entity, PhysicsBody { .friction = 0.8f });
    actors.emplace<Skin>(
        entity,
        ActorKind::Player,
        SkinType::PlayerDefault,
        dgm::Animation(atlas.atlas.getAnimationStates(atlas.playerLocation), 8),
        sf::Vector2f { 0.f, -10.f });
    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
    actors.emplace<Health>(entity, 100);
    actors.emplace<WeaponInventory>(
        entity,
        0,
        std::vector<Weapon> {
            WeaponBuilder::createWeapon({ WeaponModule::CadenceBarrel,
                                          WeaponModule::ExplosiveAmmo,
                                          WeaponModule::Ricochet }),
            WeaponBuilder::createWeapon(
                { WeaponModule::SpreadBarrel, WeaponModule::Spikes }) });
    actors.emplace<EntityInput>(entity, std::make_unique<PlayerInput>(input));

    actors.get<Skin>(entity).animation.setState("idle-front", "looping"_true);

    return entity;
}

entt::entity ActorBuilder::createNpc(
    entt::registry& actors,
    const sf::Vector2f& spawnPosition,
    const GameScene& scene,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();

    actors.emplace<Collider>(entity, dgm::Circle(spawnPosition, 8.f));
    actors.emplace<PhysicsBody>(entity, PhysicsBody { .friction = 0.8f });
    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = ActorKind::Npc,
            .skinType = SkinType::Bighead,
            .animation = dgm::Animation(
                atlas.atlas.getAnimationStates(atlas.bigheadLocation), 8),
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
        });
    actors.emplace<LookDirection>(entity, sf::Vector2f { 1.f, 0.f });
    actors.emplace<Health>(entity, 100);
    // TODO: melee weapon
    actors.emplace<WeaponInventory>(
        entity, 0, std::vector<Weapon> { WeaponBuilder::createWeapon({}) });
    actors.emplace<EntityInput>(
        entity, std::make_unique<NpcInput>(scene, entity));

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

    return { 0.f, 0.f };
}

entt::entity ActorBuilder::createProp(
    entt::registry& actors,
    const sf::Vector2f& origin,
    size_t propId,
    const GameTextureAtlas& atlas)
{
    auto entity = actors.create();
    actors.emplace<Collider>(entity, getPropCollider(origin, propId));
    actors.emplace<PhysicsBody>(entity);
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
        return "cantina-table";
    };

    actors.get<Skin>(entity).animation.setState(
        stateName(propId), "looping"_true);

    return entity;
}

entt::entity ActorBuilder::createProjectile(
    entt::registry& actors,
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    const GameTextureAtlas& atlas,
    const Weapon& weapon)
{
    auto entity = actors.create();
    actors.emplace<Collider>(
        entity,
        dgm::Circle(origin, 3.f),
        "reportMeshCollisions"_true,
        "reportActorCollisions"_true,
        "nonblocking"_true);
    actors.emplace<PhysicsBody>(
        entity,
        PhysicsBody {
            .forward = direction * weapon.projectileSpeed,
            .bounciness = 0.8f,
            .friction = weapon.defaultProjectileInventory.traits
                                & ProjectileTraits::Shrapnels
                            ? 0.01f
                            : 0.f,
            .useAltMesh = true,
        });
    actors.emplace<Lifetime>(entity, BASE_PROJECTILE_LIFETIME);

    auto animation =
        dgm::Animation(atlas.atlas.getAnimationStates(atlas.bulletLocation), 8);
    animation.setState("idle", "looping"_true);
    actors.emplace<Skin>(
        entity,
        Skin {
            .kind = ActorKind::Projectile,
            .skinType = weapon.projectileSkin,
            .animation = std::move(animation),
        });

    actors.emplace<ProjectileInventory>(
        entity, weapon.defaultProjectileInventory);

    return entity;
}

/*
Actor ActorBuilder::createNpc(
    const sf::Vector2f& spawnPosition,
    const GameTextureAtlas& atlas,
    size_t inventoryIdx)
{

}

Actor ActorBuilder::createProjectile(
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    const GameTextureAtlas& atlas,
    const Weapon& weapon,
    size_t inventoryIdx)
{
    auto animation =
        dgm::Animation(atlas.atlas.getAnimationStates(atlas.bulletLocation), 8);
    animation.setState("idle", "looping"_true);

    return Actor {
        .kind = ActorKind::Projectile,
        .skin = weapon.projectileSkin,
        .body =
            PhysicsBody {
                .shape = dgm::Circle(origin, 3.f),
                .forward = direction * weapon.projectileSpeed,
                .options =
                    PhysicsOptions {
                        .bounciness = 0.8f,
                        .friction = weapon.defaultProjectileInventory.traits
                                            & ProjectileTraits::Shrapnels
                                        ? 0.01f
                                        : 0.f,
                        .reportMeshCollisions = true,
                        .reportActorCollisions = true,
                        .nonblocking = true,
                        .useAltMesh = true,
                    },
            },
        .lookDirection = direction,
        .animation = std::move(animation),
        .inventoryIdx = inventoryIdx,
    };
}

Actor ActorBuilder::createEffect(
    const sf::Vector2f& origin,
    const EffectType type,
    const GameTextureAtlas& atlas)
{
    if (type == EffectType::BulletDeath)
    {
        auto actor = Actor {
            .kind = ActorKind::Effect,
            .skin = SkinType::BigBullet,
            .body =
                PhysicsBody {
                    .shape = dgm::Circle(origin, 1.f),
                    .options = { .disabled = true, },
                },
            .animation = dgm::Animation(
                atlas.atlas.getAnimationStates(atlas.bulletLocation), 8),
        };

        actor.animation.setState("death", "looping"_false);

        return actor;
    }

    assert(type == EffectType::Explosion);

    auto actor = Actor {
        .kind = ActorKind::Effect,
        .skin = SkinType::Explosion,
        .body =
            PhysicsBody {
                .shape = dgm::Circle(origin, 1.f),
                .options = { .disabled = true, },
            },
        .animation = dgm::Animation(
            atlas.atlas.getAnimationStates(atlas.explosionLocation), 8),
    };

    actor.animation.setState("death", "looping"_false);

    return actor;
}

Actor ActorBuilder::createProp(
    const sf::Vector2f& origin, size_t propId, const GameTextureAtlas& atlas)
{
    auto actor = Actor {
        .kind = ActorKind::Prop,
        .skin = SkinType::Prop,
        .body = getPropBody(origin, propId),
        .spriteOriginOffsetFromCollider = getSpriteOffset(propId),
        .animation =
            dgm::Animation(atlas.atlas.getAnimationStates(atlas.propsLocation)),
    };

    auto stateName = [](size_t id)
    {
        if (id == 0)
            return "labtube-full";
        else if (id == 1)
            return "labtube";
        else if (id == 2)
            return "small-table";
        return "cantina-table";
    };

    actor.animation.setState(stateName(propId), "looping"_true);

    return actor;
}

Actor ActorBuilder::createDamageMarker(
    const sf::Vector2f& origin, const float radius, size_t inventoryIdx)
{
    return Actor {
        .kind = ActorKind::DamageMarker,
        .body =
            PhysicsBody {
                .shape = dgm::Circle(origin, radius),
                .options =
                    PhysicsOptions {
                        .reportActorCollisions = true,
                        .nonblocking = true,
                    },
            },
        .animation = dgm::Animation(NULL_ANIMATION_STATES),
        .inventoryIdx = inventoryIdx,
    };
}
*/
