#include "game/builders/ActorBuilder.hpp"
#include "types/SemanticTypes.hpp"

constexpr float PROJECTILE_SPEED = 512;

Actor ActorBuilder::createProjectile(
    const sf::Vector2f& origin,
    const sf::Vector2f& direction,
    const GameTextureAtlas& atlas,
    size_t inventoryIdx)
{
    auto animation =
        dgm::Animation(atlas.atlas.getAnimationStates(atlas.bulletLocation));
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
