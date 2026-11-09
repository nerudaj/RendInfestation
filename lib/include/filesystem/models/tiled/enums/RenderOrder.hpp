#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{

    enum class [[nodiscard]] RenderOrder
    {
        RightDown,
        RightUp,
        LeftDown,
        LeftUp,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        RenderOrder,
        {
            { RenderOrder::RightDown, "right-down" },
            { RenderOrder::RightUp, "right-up" },
            { RenderOrder::LeftDown, "left-down" },
            { RenderOrder::LeftUp, "left-up" },
        });
} // namespace tiled
