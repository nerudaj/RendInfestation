#include "game/engine/AnimationEngine.hpp"
#include "types/SemanticTypes.hpp"

void AnimationEngine::operator()(const event::PlayerFiredWeapon&)
{
    assert(scene.actors[0].kind == ActorKind::Player);
    scene.actors[0].animation.setState("attack-front");
}

void AnimationEngine::operator()(const event::ProjectileHitSomething& e)
{
    assert(scene.actors.isIndexValid(e.idx));
    scene.actors[e.idx].animation.setState("death");
}

void AnimationEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, idx] : scene.actors)
    {
        auto status = actor.animation.update(time);
        if (status != dgm::Animation::PlaybackStatus::Finished) continue;

        if (actor.kind == ActorKind::Projectile
            && actor.animation.getStateName() == "death")
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(idx);
        }
        else if (actor.kind == ActorKind::Player)
        {
            actor.animation.setState("idle-front", "looping"_true);
        }
    }
}
