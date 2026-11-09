#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{
    enum class [[nodiscard]] MapType
    {
        Map,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        MapType,
        {
            { MapType::Map, "map" },
        });
} // namespace tiled
