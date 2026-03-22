#pragma once

#include "game/Types.hpp"
#include <entt/entt.hpp>
#include <variant>

namespace event
{
    struct [[nodiscard]] ActorToMeshCollision final
    {
        entt::entity entity;

        explicit ActorToMeshCollision(entt::entity entity) : entity(entity) {}
    };

    struct [[nodiscard]] ActorToActorCollision final
    {
        entt::entity entity1;
        entt::entity entity2;

        explicit ActorToActorCollision(
            entt::entity entity1, entt::entity entity2)
            : entity1(entity1), entity2(entity2)
        {
        }
    };

    struct [[nodiscard]] ActorFiredWeapon final
    {
        entt::entity entity;

        explicit ActorFiredWeapon(entt::entity entity) : entity(entity) {}
    };

    struct [[nodiscard]] ActorStartedAttack final
    {
        entt::entity entity;

        explicit ActorStartedAttack(entt::entity entity) : entity(entity) {}
    };

    struct [[nodiscard]] ActorFinishedAttack final
    {
        entt::entity entity;

        explicit ActorFinishedAttack(entt::entity entity) : entity(entity) {}
    };

    struct [[nodiscard]] ProjectileDestroyed final
    {
        entt::entity projectileEntity;

        explicit ProjectileDestroyed(entt::entity projectileEntity)
            : projectileEntity(projectileEntity)
        {
        }
    };

    struct [[nodiscard]] ObjectDestroyed final
    {
        entt::entity entity;

        explicit ObjectDestroyed(entt::entity entity) : entity(entity) {}
    };

    struct [[nodiscard]] DoorOpened final
    {
        entt::entity doorEntity;

        explicit DoorOpened(entt::entity doorEntity) : doorEntity(doorEntity) {}
    };

    struct [[nodiscard]] DoorStartsClosing final
    {
        entt::entity doorEntity;

        explicit DoorStartsClosing(entt::entity doorEntity)
            : doorEntity(doorEntity)
        {
        }
    };

    struct [[nodiscard]] ActorIsFalling final
    {
        entt::entity entity;
        float deltaTime;

        ActorIsFalling(entt::entity _entity, float _deltaTime)
            : entity(_entity), deltaTime(_deltaTime)
        {
        }
    };
} // namespace event

using GameEvent = std::variant<
    event::ActorToMeshCollision,
    event::ActorToActorCollision,
    event::ActorFiredWeapon,
    event::ProjectileDestroyed,
    event::ObjectDestroyed,
    event::ActorStartedAttack,
    event::ActorFinishedAttack,
    event::DoorOpened,
    event::DoorStartsClosing,
    event::ActorIsFalling>;
