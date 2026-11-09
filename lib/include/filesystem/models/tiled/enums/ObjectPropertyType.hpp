#pragma once

#include <nlohmann/json.hpp>

namespace tiled
{
    enum class [[nodiscard]] ObjectPropertyType
    {
        Bool,
        Int,
        Float,
        String
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(
        ObjectPropertyType,
        {
            { ObjectPropertyType::Bool, "bool" },
            { ObjectPropertyType::Int, "int" },
            { ObjectPropertyType::Float, "float" },
            { ObjectPropertyType::String, "string" },
        });
} // namespace tiled
