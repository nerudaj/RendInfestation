#pragma once

#include "game/Types.hpp"
#include <entt/entt.hpp>
#include <variant>

namespace event
{
    struct ActorToMeshCollision
    {
        entt::entity entity;

        explicit ActorToMeshCollision(entt::entity entity) : entity(entity) {}
    };

    struct ActorToActorCollision
    {
        entt::entity entity1;
        entt::entity entity2;

        explicit ActorToActorCollision(
            entt::entity entity1, entt::entity entity2)
            : entity1(entity1), entity2(entity2)
        {
        }
    };

    struct ActorFiredWeapon
    {
        entt::entity entity;

        explicit ActorFiredWeapon(entt::entity entity) : entity(entity) {}
    };

    struct EnemyStartedAttack
    {
        entt::entity enemyEntity;

        explicit EnemyStartedAttack(entt::entity entity) : enemyEntity(entity)
        {
        }
    };

    struct EnemyAttackLands
    {
        entt::entity enemyEntity;

        explicit EnemyAttackLands(entt::entity entity) : enemyEntity(entity) {}
    };

    struct ProjectileDestroyed
    {
        entt::entity projectileEntity;

        explicit ProjectileDestroyed(entt::entity projectileEntity)
            : projectileEntity(projectileEntity)
        {
        }
    };

    struct ObjectDestroyed
    {
        entt::entity entity;

        explicit ObjectDestroyed(entt::entity entity) : entity(entity) {}
    };
} // namespace event

using GameEvent = std::variant<
    event::ActorToMeshCollision,
    event::ActorToActorCollision,
    event::ActorFiredWeapon,
    event::ProjectileDestroyed,
    event::ObjectDestroyed,
    event::EnemyStartedAttack,
    event::EnemyAttackLands>;
