#pragma once

#include "game/definitions/GameEvents.hpp"
#include "game/definitions/GameScene.hpp"
#include "misc/EventQueue.hpp"

class [[nodiscard]] PhysicsEngine final
{
public:
    PhysicsEngine(GameScene& scene, EventQueue<GameEvent>& queue)
        : scene(scene), eventQueue(queue)
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
                handleProjectileHit(actorIdx, std::nullopt);
        }

        scene.actors.removeFromLookup(actorIdx, collider);

        actor.body.move(moment);

        for (auto&& candidateIdx : scene.actors.getOverlapCandidates(collider))
        {
            auto&& candidate = scene.actors[candidateIdx];
            if (candidate.body.collidesWith(collider))
            {
                if (actor.kind == ActorKind::Projectile)
                    handleProjectileHit(actorIdx, candidateIdx);
                else
                    actor.body.move(-moment);
            }
        }

        scene.actors.returnToLookup(actorIdx, collider);

        actor.body.forward -= actor.body.forward * actor.body.friction;
    }

    void handleProjectileHit(
        size_t projectileIdx, std::optional<size_t> hitActorIdx);

private:
    GameScene& scene;
    EventQueue<GameEvent>& eventQueue;
};
