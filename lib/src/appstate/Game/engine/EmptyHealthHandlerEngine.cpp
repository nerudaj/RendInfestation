#include "appstate/Game/engine/EmptyHealthHandlerEngine.hpp"
#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/utility/EntityUtility.hpp"

void EmptyHealthHandlerEngine::update(const dgm::Time& time)
{
    for (auto&& [entity, health] : scene.actors.view<Health>().each())
    {
        if (health.value > 0) continue;

        auto skin = scene.actors.try_get<Skin>(entity);
        if (skin && skin->kind == EntityKind::Prop)
        {
            audio.playAttenuated(
                health.deathSound, scene.actors.get<Collider>(entity));
            EntityUtility::destroyProp(scene.actors, entity);
        }
        else
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(entity);
        }
    }
}
