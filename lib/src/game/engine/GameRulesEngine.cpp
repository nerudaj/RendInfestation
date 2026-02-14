#include "game/engine/GameRulesEngine.hpp"
#include <algorithm>
#include <limits>

const float SPEED = 192.f;

void GameRulesEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, _] : scene.actors)
    {
        if (actor.kind == ActorKind::Player)
        {
            assert(actor.inventoryIdx);
            assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
            updatePlayer(
                actor,
                std::get<PlayerInventory>(
                    scene.inventories[*actor.inventoryIdx]),
                time);
        }
        else if (actor.kind == ActorKind::Projectile)
        {
            assert(actor.inventoryIdx);
            assert(scene.inventories.isIndexValid(*actor.inventoryIdx));
            updateProjectile(
                actor,
                std::get<ProjectileInventory>(
                    scene.inventories[*actor.inventoryIdx]),
                time);
        }
    }
}

void GameRulesEngine::updatePlayer(
    Actor& actor, PlayerInventory& inventory, const dgm::Time& time)
{
    auto&& forwardImpulse = input.getForward();
    if (forwardImpulse.length() > 0.f)
        actor.body.forward = forwardImpulse * SPEED;

    scene.cameraPosition = actor.body.getPosition();

    if (input.isShootPressed())
    {
        auto idx = scene.actors.emplaceBack(Actor {
            .kind = ActorKind::Projectile,
            .body =
                PhysicsBody {
                    .shape = dgm::Circle(actor.body.getPosition(), 3.f),
                    .bounciness = 0.f,
                    .friction = 0.f,
                    .forward = sf::Vector2f(SPEED, actor.orientation),
                },
            .orientation = actor.orientation,
            .animation = dgm::Animation(
                atlas.atlas.getAnimationStates(atlas.bulletLocation)),
        });

        scene.actors[idx].animation.setState("idle", "looping"_true);
    }
}

void GameRulesEngine::updateProjectile(
    Actor& actor, ProjectileInventory& inventory, const dgm::Time& time)
{
    inventory.lifetime -= time.getElapsed();
}
