#pragma once

#include <variant>

namespace event
{
    struct PlayerFiredWeapon
    {
    };

    struct ProjectileHitSomething
    {
        size_t idx;

        explicit ProjectileHitSomething(size_t idx) : idx(idx) {}
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
