#pragma once

#include <variant>

namespace event
{
    struct ObjectDestroyed
    {
        size_t idx;

        explicit ObjectDestroyed(size_t idx) : idx(idx) {}
    };
}

using GameEvent = std::variant<event::ObjectDestroyed>;
