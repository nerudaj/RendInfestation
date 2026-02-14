#pragma once

#include <variant>

namespace event
{
    struct PlayerWantsToFire
    {
    };

    struct ObjectDestroyed
    {
        size_t idx;

        explicit ObjectDestroyed(size_t idx) : idx(idx) {}
    };
} // namespace event

using GameEvent =
    std::variant<event::PlayerWantsToFire, event::ObjectDestroyed>;
