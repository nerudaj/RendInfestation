#include "game/engine/AnimationEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "types/SemanticTypes.hpp"

void AnimationEngine::operator()(const event::PlayerFiredWeapon&)
{
    assert(scene.actors[0].kind == ActorKind::Player);
    scene.actors[0].animation.setState("attack-front");
}

void AnimationEngine::operator()(const event::ProjectileDestroyed& e)
{
    auto&& [projectile, inventory] =
        getActorAndInventory<ProjectileInventory>(scene, e.projectileIdx);

    // Spawn projectile death effect
    scene.actors.emplaceBack(ActorBuilder::createEffect(
        scene.actors[e.projectileIdx].body.getPosition(),
        inventory.traits & ProjectileTraits::Explosive
            ? EffectType::Explosion
            : EffectType::BulletDeath,
        atlas));
}

void AnimationEngine::operator()(const event::EnemyStartedAttack& e)
{
    auto&& npc = scene.actors[e.enemyIdx];
    npc.animation.setState("attack-windup-front", "looping"_false);
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
        else if (actor.kind == ActorKind::Npc)
        {
            if (actor.animation.getStateName() == "attack-windup-front")
            {
                eventQueue.pushEvent<event::EnemyAttackLands>(idx);
                actor.animation.setState("attack-punch-front", "looping"_false);
            }
            else if (actor.animation.getStateName() == "attack-punch-front")
            {
                actor.animation.setState(
                    "attack-recovery-front", "looping"_false);
            }
            else
                actor.animation.setState("walk-front", "looping"_true);
        }
        else if (actor.kind == ActorKind::Effect)
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(idx);
        }
    }
}
