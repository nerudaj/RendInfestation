#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{
    enum class [[nodiscard]] Orientation
    {
        Orthogonal,
        Isometric,
        Staggered,
        Hexagonal,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        Orientation,
        {
            { Orientation::Orthogonal, "orthogonal" },
            { Orientation::Isometric, "isometric" },
            { Orientation::Staggered, "staggered" },
            { Orientation::Hexagonal, "hexagonal" },
        });
} // namespace tiled
