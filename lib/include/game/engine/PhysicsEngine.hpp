#pragma once

#include "game/Types.hpp"
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
        const dgm::Time& time,
        Actor& actor,
        ActorIndexType actorIdx,
        const T& collider)
    {
        if (actor.kind == ActorKind::Effect) return;

        auto moment = actor.body.forward * time.getDeltaTime();

        if (dgm::Collision::advanced(scene.levelMesh, collider, moment))
        {
            if (actor.kind == ActorKind::Projectile)
                handleProjectileEnvironmentHit(actorIdx, actor.body, moment);
        }

        spatialIndex.removeFromLookup(actorIdx, collider);

        for (auto&& candidateIdx : spatialIndex.getOverlapCandidates(collider))
        {
            auto&& candidate = scene.actors[candidateIdx];
            if (candidate.kind == ActorKind::Projectile
                && actor.kind == candidate.kind)
                continue;

            const auto hasCollision = [&]
            {
                if (candidate.kind == ActorKind::DamageMarker)
                    return candidate.body.collidesWith(collider);

                return std::visit(
                    overloads { [&](const auto& anchor)
                                {
                                    return dgm::Collision::advanced(
                                        anchor, collider, moment);
                                } },
                    candidate.body.shape);
            }();

            if (hasCollision)
            {
                if (actor.kind == ActorKind::Projectile)
                    eventQueue.pushEvent<event::ProjectileHitSomething>(
                        actorIdx, candidateIdx);
                else if (actor.kind == ActorKind::DamageMarker)
                    eventQueue.pushEvent<event::ActorDamaged>(
                        actorIdx, candidateIdx);
            }
        }

        actor.body.move(moment);

        spatialIndex.returnToLookup(actorIdx, collider);

        actor.body.forward -= actor.body.forward * actor.body.friction;
    }

    void handleProjectileEnvironmentHit(
        ActorIndexType projectileIdx,
        PhysicsBody& body,
        const sf::Vector2f& moment);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
    dgm::SpatialIndex<ActorIndexType> spatialIndex;
};
