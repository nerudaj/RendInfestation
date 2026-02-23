#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] PhysicsEngine final
{
public:
    PhysicsEngine(GameScene& scene, EventQueue<GameEvent>& queue)
        : scene(scene), eventQueue(queue), spatialIndex(scene.levelBounds, 128)
    {
    }

public:
    void operator()(const auto&) {}

public:
    void update(const dgm::Time& time);

    template<class T>
        requires std::same_as<T, dgm::Circle> || std::same_as<T, dgm::Rect>
    void updateForConcreteCollider(
        const dgm::Time& time, Actor& actor, size_t actorIdx, const T& collider)
    {
        if (actor.kind == ActorKind::Effect) return;

        auto moment = actor.body.forward * time.getDeltaTime();

        if (dgm::Collision::advanced(scene.levelMesh, collider, moment))
        {
            if (actor.kind == ActorKind::Projectile)
                handleProjectileEnvironmentHit(actorIdx, actor.body, moment);
        }

        spatialIndex.removeFromLookup(actorIdx, collider);

        actor.body.move(moment);

        for (auto&& candidateIdx : spatialIndex.getOverlapCandidates(collider))
        {
            auto&& candidate = scene.actors[candidateIdx];
            if (candidate.kind == ActorKind::Projectile
                && actor.kind == candidate.kind)
                continue;

            if (candidate.body.collidesWith(collider))
            {
                if (actor.kind == ActorKind::Projectile)
                    eventQueue.pushEvent<event::ProjectileHitSomething>(
                        actorIdx, candidateIdx);
                else
                    actor.body.move(-moment);
            }
        }

        spatialIndex.returnToLookup(actorIdx, collider);

        actor.body.forward -= actor.body.forward * actor.body.friction;
    }

    void handleProjectileEnvironmentHit(
        size_t projectileIdx, PhysicsBody& body, const sf::Vector2f& moment);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
    dgm::SpatialIndex<> spatialIndex;
};
