#pragma once

#include <variant>

namespace event
{
    struct PlayerFiredWeapon
    {
    };

    struct ProjectileHitSomething
    {
        size_t projectileIdx;
        std::optional<size_t> hitActorIdx;

        explicit ProjectileHitSomething(
            size_t projectileIdx, std::optional<size_t> hitActorIdx)
            : projectileIdx(projectileIdx), hitActorIdx(hitActorIdx)
        {
        }
    };

    struct ObjectDestroyed
    {
        size_t idx;

        explicit ObjectDestroyed(size_t idx) : idx(idx) {}
    };
} // namespace event

using GameEvent = std::variant<
    event::PlayerFiredWeapon,
    event::ProjectileHitSomething,
    event::ObjectDestroyed>;
