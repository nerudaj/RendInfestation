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
        .body =
            PhysicsBody {
                .shape = dgm::Circle(origin, 3.f),
                .bounciness = 0.f,
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
    assert(type == EffectType::BulletDeath);

    auto actor = Actor {
        .kind = ActorKind::Effect,
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

Actor ActorBuilder::createProp(
    const sf::Vector2f& origin, size_t propId, const GameTextureAtlas& atlas)
{
    auto actor = Actor {
        .kind = ActorKind::Prop,
        .body = dgm::Rect({ origin.x, origin.y - 64.f }, { 64.f, 64.f }),
        .animation = dgm::Animation(atlas.propsStates),
    };

    actor.animation.setState(std::format("idle-{}", propId), "looping"_true);

    return actor;
}
