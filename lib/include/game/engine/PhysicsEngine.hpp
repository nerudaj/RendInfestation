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
        if (actor.body.options.disabled) return;

        auto moment = actor.body.forward * time.getDeltaTime();

        if (dgm::Collision::advanced(
                actor.body.options.useAltMesh ? scene.altLevelMesh
                                              : scene.levelMesh,
                collider,
                moment))
        {
            if (actor.body.options.reportMeshCollisions)
                eventQueue.pushEvent<event::ActorToMeshCollision>(actorIdx);

            if (moment.x == 0.f)
                actor.body.forward.x *= actor.body.options.bounciness;
            if (moment.y == 0.f)
                actor.body.forward.y *= actor.body.options.bounciness;
        }

        spatialIndex.removeFromLookup(actorIdx, collider);

        for (auto&& candidateIdx : spatialIndex.getOverlapCandidates(collider))
        {
            auto&& candidate = scene.actors[candidateIdx];

            const auto hasCollision = [&]
            {
                if (candidate.body.options.nonblocking)
                    return candidate.body.collidesWith(collider);

                return std::visit(
                    overloads { [&](const auto& anchor)
                                {
                                    return dgm::Collision::advanced(
                                        anchor, collider, moment);
                                } },
                    candidate.body.shape);
            }();

            const bool reportCollision =
                actor.body.options.reportActorCollisions
                || candidate.body.options.reportActorCollisions;
            if (hasCollision && reportCollision)
            {
                eventQueue.pushEvent<event::ActorToActorCollision>(
                    actorIdx, candidateIdx);
            }
        }

        actor.body.move(moment);

        spatialIndex.returnToLookup(actorIdx, collider);

        actor.body.forward -= actor.body.forward * actor.body.options.friction;
    }

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
    dgm::SpatialIndex<ActorIndexType> spatialIndex;
};
