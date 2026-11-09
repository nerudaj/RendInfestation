#pragma once

#include "appstate/Game/Types.hpp"
#include "appstate/Game/definitions/Components.hpp"
#include <entt/entt.hpp>
#include <variant>

namespace event
{
    struct [[nodiscard]] ActorToMeshCollision final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] ActorToActorCollision final
    {
        entt::entity entity1;
        entt::entity entity2;
    };

    struct [[nodiscard]] ActorBounced final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] ActorFiredWeapon final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] ActorStartedAttack final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] ActorFinishedAttack final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] ProjectileDestroyed final
    {
        entt::entity projectileEntity;
    };

    struct [[nodiscard]] ObjectDestroyed final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] DoorOpened final
    {
        entt::entity doorEntity;
    };

    struct [[nodiscard]] DoorStartsClosing final
    {
        entt::entity doorEntity;
    };

    struct [[nodiscard]] ActorIsFalling final
    {
        entt::entity entity;
        float deltaTime;
    };

    struct [[nodiscard]] ActorMoved final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] ActorStopped final
    {
        entt::entity entity;
    };

    struct [[nodiscard]] ActorDamaged final
    {
        entt::entity entity;
        int damageAmount;
        sf::Vector2f impactForce;
    };

    struct [[nodiscard]] WorkbenchOpened final
    {
    };

    struct [[nodiscard]] NoteOpened final
    {
        int noteId;
    };

    struct [[nodiscard]] WaveEnded final
    {
    };

    struct [[nodiscard]] SurvivalSpawnerTimerHit final
    {
        SkinType typeToSpawn;
    };

    struct [[nodiscard]] PlayerDied final
    {
    };
} // namespace event

using GameEvent = std::variant<
    event::ActorToMeshCollision,
    event::ActorToActorCollision,
    event::ActorBounced,
    event::ActorFiredWeapon,
    event::ProjectileDestroyed,
    event::ObjectDestroyed,
    event::ActorStartedAttack,
    event::ActorFinishedAttack,
    event::DoorOpened,
    event::DoorStartsClosing,
    event::ActorIsFalling,
    event::ActorMoved,
    event::ActorStopped,
    event::ActorDamaged,
    event::WorkbenchOpened,
    event::NoteOpened,
    event::WaveEnded,
    event::SurvivalSpawnerTimerHit,
    event::PlayerDied>;
