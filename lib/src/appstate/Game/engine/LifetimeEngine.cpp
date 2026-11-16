#include "appstate/Game/engine/LifetimeEngine.hpp"
#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/utility/EntityUtility.hpp"

void LifetimeEngine::update(const dgm::Time& time)
{
    for (auto&& [entity, lifetime] : scene.actors.view<Lifetime>().each())
    {
        lifetime.get() -= time.getElapsed();
        if (lifetime.get() > sf::Time::Zero) continue;

        auto inventory = scene.actors.try_get<ProjectileInventory>(entity);
        if (inventory && inventory->traits & ProjectileTraits::Turret)
        {
            ActorBuilder::createTurret(
                scene.actors,
                scene.actors.get<Collider>(entity).getPosition(),
                atlas,
                *inventory);
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
        }
        else if (inventory)
        {
            EntityUtility::spawnDamageMarkerForProjectile(
                scene.actors, entity, inventory);
            eventQueue.pushEvent<event::ProjectileDestroyed>(entity);
        }
        else
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
        }
    }
}
