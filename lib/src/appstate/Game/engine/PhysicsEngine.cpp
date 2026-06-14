#include "appstate/Game/engine/PhysicsEngine.hpp"

void PhysicsEngine::update(const dgm::Time& time)
{
    spatialIndex.clear();
    for (auto&& [entity, collider] : scene.actors.view<Collider>().each())
    {
        if (collider.options.disabled) continue;

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

    if (body.useAltMesh)
    {
        const auto tilePos =
            sf::Vector2i(collider.getPosition().componentWiseDiv(
                sf::Vector2f(scene.altLevelMesh.getVoxelSize())));

        const auto tile =
            scene.altLevelMesh
                [tilePos.y * scene.altLevelMesh.getDataSize().x + tilePos.x];
        if (tile < 0 && body.canFall)
        {
            eventQueue.pushEvent<event::ActorIsFalling>(
                entity, time.getDeltaTime());
        }
    }

    if (!options.nonblocking)
    {
        performEntityCollisionDetection(
            entity, collider, moment, options.semighost);
    }

    collider.move(moment);

    body.forward -= body.forward * body.friction;
}

template<class T>
    requires std::same_as<T, dgm::Circle> || std::same_as<T, dgm::Rect>
void PhysicsEngine::performEntityCollisionDetection(
    entt::entity entity, T& collider, sf::Vector2f& moment, bool isSemighost)
{
    spatialIndex.removeFromLookup(entity, collider);

    for (auto&& candidate : spatialIndex.getOverlapCandidates(collider))
    {
        auto&& candidateCollider = scene.actors.get<Collider>(candidate);
        if (isSemighost && candidateCollider.options.semighost) continue;

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
            candidateCollider.options.reportActorCollisions;
        if (hasCollision && reportCollision)
        {
            // Invariant - entity should never be a projectile nor a damage
            // marker while candidate is most definitely either projectile or
            // damage marker
            eventQueue.pushEvent<event::ActorToActorCollision>(
                entity, candidate);
        }
    }

    spatialIndex.returnToLookup(entity, collider);
}
