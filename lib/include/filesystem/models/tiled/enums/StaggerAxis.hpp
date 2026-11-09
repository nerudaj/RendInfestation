#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{
    enum class [[nodiscard]] StaggerAxis
    {
        X,
        Y,
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        StaggerAxis,
        {
            { StaggerAxis::X, "x" },
            { StaggerAxis::Y, "y" },
        });
} // namespace tiled
