#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{
    enum class [[nodiscard]] StaggerIndex
    {
        Odd,
        Even,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        StaggerIndex,
        {
            { StaggerIndex::Odd, "odd" },
            { StaggerIndex::Even, "even" },
        });
} // namespace tiled
