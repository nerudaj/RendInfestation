#include "game/builders/ActorBuilder.hpp"
#include "types/SemanticTypes.hpp"

constexpr float PROJECTILE_SPEED = 512;

Actor ActorBuilder::createPlayer(
    const sf::Vector2f& spawnPosition,
    const GameTextureAtlas& atlas,
    size_t inventoryIdx)
{
    auto actor = Actor {
        .kind = ActorKind::Player,
        .skin = ActorSkin::PlayerDefault,
        .body =
            PhysicsBody {
                .shape = dgm::Circle(spawnPosition, 8.f),
                .friction = 0.8f,
            },
        .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
        .animation = dgm::Animation(
            atlas.atlas.getAnimationStates(atlas.playerLocation), 8),
        .inventoryIdx = inventoryIdx,
    };

    actor.animation.setState("idle-front", "looping"_true);

    return actor;
}

Actor ActorBuilder::createNpc(
    const sf::Vector2f& spawnPosition,
    const GameTextureAtlas& atlas,
    size_t inventoryIdx)
{
    auto actor = Actor {
        .kind = ActorKind::Npc,
        .skin = ActorSkin::Bighead,
        .body =
            PhysicsBody {
                .shape = dgm::Circle(spawnPosition, 8.f),
                .friction = 0.8f,
            },
        .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
        .animation = dgm::Animation(
            atlas.atlas.getAnimationStates(atlas.bigheadLocation), 8),
        .inventoryIdx = inventoryIdx,
    };

    actor.animation.setState("walk-front", "looping"_true);

    return actor;
}

Actor ActorBuilder::createProjectile(
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    const GameTextureAtlas& atlas,
    size_t inventoryIdx)
{
    auto animation =
        dgm::Animation(atlas.atlas.getAnimationStates(atlas.bulletLocation), 8);
    animation.setState("idle", "looping"_true);

    return Actor {
        .kind = ActorKind::Projectile,
        .skin = ActorSkin::BigBullet,
        .body =
            PhysicsBody {
                .shape = dgm::Circle(origin, 3.f),
                .bounciness = 0.8f,
                .friction = 0.f,
                .forward = direction * PROJECTILE_SPEED,
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
            .skin = ActorSkin::BigBullet,
            .body =
                PhysicsBody {
                    .shape = dgm::Circle(origin, 1.f),
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
        .skin = ActorSkin::Explosion,
        .body =
            PhysicsBody {
                .shape = dgm::Circle(origin, 1.f),
            },
        .animation = dgm::Animation(
            atlas.atlas.getAnimationStates(atlas.explosionLocation), 8),
    };

    actor.animation.setState("death", "looping"_false);

    return actor;
}

static PhysicsBody getPropBody(const sf::Vector2f& origin, const size_t propId)
{
    if (propId == 0 || propId == 1)
    {
        return PhysicsBody { dgm::Circle(
            { origin.x + 32.f, origin.y - 16.f }, 13.f) };
    }
    else if (propId == 2)
    {
        return PhysicsBody { dgm::Rect(
            { origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }) };
    }

    return PhysicsBody { dgm::Rect(
        { origin.x, origin.y - 64.f }, { 64.f, 64.f }) };
}

static sf::Vector2f getSpriteOffset(const size_t propId)
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

Actor ActorBuilder::createProp(
    const sf::Vector2f& origin, size_t propId, const GameTextureAtlas& atlas)
{
    auto actor = Actor {
        .kind = ActorKind::Prop,
        .skin = ActorSkin::Prop,
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
