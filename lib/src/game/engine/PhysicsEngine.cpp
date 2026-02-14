#include "game/engine/PhysicsEngine.hpp"

void PhysicsEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, idx] : scene.actors)
    {
        auto&& body = actor.body;
        auto moment = body.forward * time.getDeltaTime();

        // TODO: only bounce with the bouncy trait,
        // otherwise destroy projectile
        if (std::visit(
                [&](const auto& shape)
                {
                    return dgm::Collision::advanced(
                        scene.levelMesh, shape, moment);
                },
                body.shape)
            && actor.kind == ActorKind::Projectile)
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(idx);

            // bounce code
            // body.forward *= -body.bounciness;
            // moment = body.forward * time.getDeltaTime();
        }

        body.move(moment);
        body.forward -= body.forward * body.friction;
    }
}
