#pragma once

#include "game/Types.hpp"
#include <variant>

namespace event
{
    struct ActorToMeshCollision
    {
        ActorIndexType idx;

        explicit ActorToMeshCollision(ActorIndexType idx) : idx(idx) {}
    };

    struct ActorToActorCollision
    {
        ActorIndexType actor1;
        ActorIndexType actor2;

        explicit ActorToActorCollision(
            ActorIndexType actor1, ActorIndexType actor2)
            : actor1(actor1), actor2(actor2)
        {
        }
    };

    struct PlayerFiredWeapon
    {
    };

    struct EnemyStartedAttack
    {
        ActorIndexType enemyIdx;

        explicit EnemyStartedAttack(ActorIndexType idx) : enemyIdx(idx) {}
    };

    struct EnemyAttackLands
    {
        ActorIndexType enemyIdx;

        explicit EnemyAttackLands(ActorIndexType idx) : enemyIdx(idx) {}
    };

    struct ProjectileDestroyed
    {
        ActorIndexType projectileIdx;

        explicit ProjectileDestroyed(ActorIndexType projectileIdx)
            : projectileIdx(projectileIdx)
        {
        }
    };

    struct ObjectDestroyed
    {
        ActorIndexType idx;

        explicit ObjectDestroyed(ActorIndexType idx) : idx(idx) {}
    };
} // namespace event

using GameEvent = std::variant<
    event::ActorToMeshCollision,
    event::ActorToActorCollision,
    event::PlayerFiredWeapon,
    event::ProjectileDestroyed,
    event::ObjectDestroyed,
    event::EnemyStartedAttack,
    event::EnemyAttackLands>;
