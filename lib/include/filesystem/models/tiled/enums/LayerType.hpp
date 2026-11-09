#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{
    enum class [[nodiscard]] LayerType
    {
        Uninitialized = -1,
        TileLayer = 0,
        ObjectGroup,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        LayerType,
        {
            { LayerType::TileLayer, "tilelayer" },
            { LayerType::ObjectGroup, "objectgroup" },
        });
} // namespace tiled
