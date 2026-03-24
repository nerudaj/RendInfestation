#include "game/engine/AnimationEngine.hpp"
#include "game/builders/ActorBuilder.hpp"
#include "types/SemanticTypes.hpp"

void AnimationEngine::operator()(const event::ActorFiredWeapon& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    skin.animation.setState("attack-front", "looping"_false);
}

void AnimationEngine::operator()(const event::ProjectileDestroyed& e)
{
    auto&& [collider, inventory] =
        scene.actors.get<Collider, ProjectileInventory>(e.projectileEntity);

    ActorBuilder::createEffect(
        scene.actors,
        collider.getPosition(),
        inventory.traits & ProjectileTraits::Explosive
            ? EffectType::Explosion
            : EffectType::BulletDeath,
        atlas,
        inventory.traits & ProjectileTraits::Big ? 2.f : 1.f);
}

void AnimationEngine::operator()(const event::ActorStartedAttack& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == ActorKind::Npc)
    {
        skin.animation.setState("attack-windup-front", "looping"_false);
    }
}

void AnimationEngine::operator()(const event::ActorFinishedAttack& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == ActorKind::Npc)
    {
        skin.animation.setState("attack-recovery-front", "looping"_false);
    }
    else if (skin.kind == ActorKind::Player)
    {
        skin.animation.setState("idle-front", "looping"_true);
    }
}

void AnimationEngine::operator()(const event::ActorMoved& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == ActorKind::Player || skin.kind == ActorKind::Npc)
    {
        if (skin.animation.getStateName() == "idle-front")
            skin.animation.setState("walk-front", "looping"_true);
    }
}

void AnimationEngine::operator()(const event::ActorStopped& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == ActorKind::Player || skin.kind == ActorKind::Npc)
    {
        if (skin.animation.getStateName() == "walk-front")
            skin.animation.setState("idle-front", "looping"_true);
    }
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
            eventQueue.pushEvent<event::ActorFinishedAttack>(actor);
        }
        else if (skin.kind == ActorKind::Npc)
        {
            if (skin.animation.getStateName() == "attack-windup-front")
            {
                eventQueue.pushEvent<event::ActorFiredWeapon>(actor);
            }
            else if (skin.animation.getStateName() == "attack-front")
            {
                eventQueue.pushEvent<event::ActorFinishedAttack>(actor);
            }
            else
                skin.animation.setState("walk-front", "looping"_true);
        }
        else if (skin.kind == ActorKind::Effect)
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(actor);
        }
        else if (skin.kind == ActorKind::Door)
        {
            if (skin.animation.getStateName() == DOOR_OPENING_ANIMATION_STATE)
            {
                skin.animation.setState(
                    DOOR_OPEN_ANIMATION_STATE, "looping"_true);
                eventQueue.pushEvent<event::DoorOpened>(actor);
            }
            else if (
                skin.animation.getStateName() == DOOR_CLOSING_ANIMATION_STATE)
            {
                skin.animation.setState(
                    DOOR_CLOSED_ANIMATION_STATE, "looping"_true);
            }
        }
    }
}
