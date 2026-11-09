#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{
    enum class [[nodiscard]] DrawOrder
    {
        Uninitialized = -1,
        TopDown = 0,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        DrawOrder, { { DrawOrder::TopDown, "topdown" } });
} // namespace tiled
