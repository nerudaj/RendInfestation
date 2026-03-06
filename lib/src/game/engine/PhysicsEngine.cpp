#include "game/engine/PhysicsEngine.hpp"

void PhysicsEngine::update(const dgm::Time& time)
{
    spatialIndex.clear();
    for (auto&& [entity, collider] : scene.actors.view<Collider>().each())
    {
        std::visit(
            [&](const auto& shape)
            { spatialIndex.returnToLookup(entity, shape); },
            collider.shape);
    }

    for (auto&& [entity, collider, physicsBody] :
         scene.actors.view<Collider, PhysicsBody>().each())
    {
        std::visit(
            [&](auto& shape)
            {
                updateForConcreteCollider(
                    time, entity, shape, collider.options, physicsBody);
            },
            collider.shape);
    }
}

template<class T>
    requires std::same_as<T, dgm::Circle> || std::same_as<T, dgm::Rect>
void PhysicsEngine::updateForConcreteCollider(
    const dgm::Time& time,
    entt::entity entity,
    T& collider,
    const ColliderOptions& options,
    PhysicsBody& body)

{
    auto moment = body.forward * time.getDeltaTime();

    if (dgm::Collision::advanced(
            body.useAltMesh ? scene.altLevelMesh : scene.levelMesh,
            collider,
            moment))
    {
        if (options.reportMeshCollisions)
            eventQueue.pushEvent<event::ActorToMeshCollision>(entity);

        if (moment.x == 0.f) body.forward.x *= -body.bounciness;
        if (moment.y == 0.f) body.forward.y *= -body.bounciness;
    }

    spatialIndex.removeFromLookup(entity, collider);

    for (auto&& candidate : spatialIndex.getOverlapCandidates(collider))
    {
        auto&& candidateCollider = scene.actors.get<Collider>(candidate);

        const auto hasCollision = [&]
        {
            if (candidateCollider.options.nonblocking)
                return candidateCollider.collidesWith(collider);

            return std::visit(
                overloads { [&](const auto& anchor)
                            {
                                return dgm::Collision::advanced(
                                    anchor, collider, moment);
                            } },
                candidateCollider.shape);
        }();

        const bool reportCollision =
            options.reportActorCollisions
            || candidateCollider.options.reportActorCollisions;
        if (hasCollision && reportCollision)
        {
            eventQueue.pushEvent<event::ActorToActorCollision>(
                entity, candidate);
        }
    }

    collider.move(moment);

    spatialIndex.returnToLookup(entity, collider);

    body.forward -= body.forward * body.friction;
}
