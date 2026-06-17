#include "appstate/Game/engine/AnimationEngine.hpp"
#include "appstate/Game/builders/ActorBuilder.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "types/SemanticTypes.hpp"

void AnimationEngine::operator()(const event::ActorFiredWeapon& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    skin.animation.setState(ATTACK_ANIMATION_STATE, "looping"_false);
}

void AnimationEngine::operator()(const event::ProjectileDestroyed& e)
{
    auto&& [collider, body, inventory, skin] =
        scene.actors.get<Collider, PhysicsBody, ProjectileInventory, Skin>(
            e.projectileEntity);

    if (inventory.traits & ProjectileTraits::Explosive)
    {
        ActorBuilder::createParticleSystem(
            scene.actors,
            collider.getPosition(),
            sf::Vector2 { 1.f, 0.f },
            inventory.traits & ProjectileTraits::Big
                ? ParticleSystemKind::HugeExplosion
                : ParticleSystemKind::Explosion);
        /*ActorBuilder::createEffect(
            scene.actors,
            collider.getPosition(),
            EffectType::Explosion,
            atlas,
            inventory.traits & ProjectileTraits::Big ? 2.f : 1.f);*/
    }
    else if (skin.skinType == SkinType::Fireball)
    {
        ActorBuilder::createParticleSystem(
            scene.actors,
            collider.getPosition(),
            dgm::Math::toUnit(body.forward),
            ParticleSystemKind::FireballExplosion);
        /*ActorBuilder::createEffect(
            scene.actors,
            collider.getPosition(),
            EffectType::FireballDecay,
            atlas,
            inventory.traits & ProjectileTraits::Big ? 2.f : 1.f);*/
    }
    else
    {
        ActorBuilder::createParticleSystem(
            scene.actors,
            collider.getPosition(),
            dgm::Math::toUnit(body.forward),
            ParticleSystemKind::ProjectileImpact);
    }
}

void AnimationEngine::operator()(const event::ActorStartedAttack& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == EntityKind::Npc)
    {
        skin.animation.setState(ATTACK_WINDUP_ANIMATION_STATE, "looping"_false);
    }
}

void AnimationEngine::operator()(const event::ActorFinishedAttack& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == EntityKind::Npc)
    {
        skin.animation.setState(
            ATTACK_RECOVERY_ANIMATION_STATE, "looping"_false);
    }
    else if (skin.kind == EntityKind::Player)
    {
        skin.animation.setState(IDLE_ANIMATION_STATE, "looping"_true);
    }
}

void AnimationEngine::operator()(const event::ActorMoved& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == EntityKind::Player || skin.kind == EntityKind::Npc)
    {
        if (skin.animation.getStateName() == IDLE_ANIMATION_STATE)
            skin.animation.setState(WALK_ANIMATION_STATE, "looping"_true);
    }
}

void AnimationEngine::operator()(const event::ActorStopped& e)
{
    auto&& skin = scene.actors.get<Skin>(e.entity);
    if (skin.kind == EntityKind::Player || skin.kind == EntityKind::Npc)
    {
        if (skin.animation.getStateName() == WALK_ANIMATION_STATE)
            skin.animation.setState(IDLE_ANIMATION_STATE, "looping"_true);
    }
}

void AnimationEngine::operator()(const event::ActorDamaged& e)
{
    try
    {
        scene.actors.get<Skin>(e.entity).animation.setState(
            HURT_ANIMATION_STATE, "looping"_false);
    }
    catch (...)
    {
        // props don't have animation states
    }
}

void AnimationEngine::update(const dgm::Time& time)
{
    for (auto&& [actor, skin] : scene.actors.view<Skin>().each())
    {
        auto status = skin.animation.update(time);

        if (status != dgm::Animation::PlaybackStatus::Finished) continue;

        if (skin.animation.getStateName() == DEATH_ANIMATION_STATE)
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(actor);
        }
        else if (skin.kind == EntityKind::Player)
        {
            eventQueue.pushEvent<event::ActorFinishedAttack>(actor);
        }
        else if (skin.kind == EntityKind::Npc)
        {
            if (skin.animation.getStateName() == ATTACK_WINDUP_ANIMATION_STATE)
            {
                eventQueue.pushEvent<event::ActorFiredWeapon>(actor);
            }
            else if (skin.animation.getStateName() == ATTACK_ANIMATION_STATE)
            {
                eventQueue.pushEvent<event::ActorFinishedAttack>(actor);
            }
            else
                skin.animation.setState(WALK_ANIMATION_STATE, "looping"_true);
        }
        else if (skin.kind == EntityKind::Effect)
        {
            eventQueue.pushEvent<event::ObjectDestroyed>(actor);
        }
        else if (skin.kind == EntityKind::Door)
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
