#pragma once

#include "game/Types.hpp"
#include <variant>

namespace event
{
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

    struct ProjectileHitSomething

    {
        ActorIndexType projectileIdx;
        std::optional<ActorIndexType> hitActorIdx;

        ProjectileHitSomething(
            ActorIndexType projectileIdx,
            std::optional<ActorIndexType> hitActorIdx)
            : projectileIdx(projectileIdx), hitActorIdx(hitActorIdx)
        {
        }
    };

    struct ActorDamaged
    {
        ActorIndexType markerIdx;
        ActorIndexType hitActorIdx;

        ActorDamaged(ActorIndexType markerIdx, ActorIndexType hitActorIdx)
            : markerIdx(markerIdx), hitActorIdx(hitActorIdx)
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
    event::PlayerFiredWeapon,
    event::ProjectileHitSomething,
    event::ObjectDestroyed,
    event::EnemyStartedAttack,
    event::EnemyAttackLands,
    event::ActorDamaged>;
