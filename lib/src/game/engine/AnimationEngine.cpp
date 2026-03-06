#include "game/engine/AnimationEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "types/SemanticTypes.hpp"

void AnimationEngine::operator()(const event::ActorFiredWeapon&)
{
    /*scene.actors.get<Skin>(scene.playerEntity)
        .animation.setState("attack-front");*/
}

void AnimationEngine::operator()(const event::ProjectileDestroyed& e)
{
    /* TODO: this
    auto&& [projectile, inventory] =
        getActorAndInventory<ProjectileInventory>(scene, e.projectileIdx);

    // Spawn projectile death effect
    scene.actors.emplaceBack(ActorBuilder::createEffect(
        scene.actors[e.projectileIdx].body.getPosition(),
        inventory.traits & ProjectileTraits::Explosive
            ? EffectType::Explosion
            : EffectType::BulletDeath,
        atlas));
    */
}

void AnimationEngine::operator()(const event::EnemyStartedAttack& e)
{
    scene.actors.get<Skin>(e.enemyEntity)
        .animation.setState("attack-windup-front", "looping"_false);
}

void AnimationEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, skin] : scene.actors.view<Skin>().each())
    {
        auto status = skin.animation.update(time);

        if (status != dgm::Animation::PlaybackStatus::Finished) continue;

        if (skin.animation.getStateName() == "death")
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(actor);
        }
        else if (skin.kind == ActorKind::Player)
        {
            skin.animation.setState("idle-front", "looping"_true);
        }
        else if (skin.kind == ActorKind::Npc)
        {
            if (skin.animation.getStateName() == "attack-windup-front")
            {
                eventQueue.pushEvent<event::EnemyAttackLands>(actor);
                skin.animation.setState("attack-punch-front", "looping"_false);
            }
            else if (skin.animation.getStateName() == "attack-punch-front")
            {
                skin.animation.setState(
                    "attack-recovery-front", "looping"_false);
            }
            else
                skin.animation.setState("walk-front", "looping"_true);
        }
        else if (skin.kind == ActorKind::Effect)
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(actor);
        }
    }
}
