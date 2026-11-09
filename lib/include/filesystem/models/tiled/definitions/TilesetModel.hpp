#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace tiled
{
    struct TilesetModel
    {
        unsigned firstgid = 0;
        std::string source = "";
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TilesetModel, firstgid, source);
} // namespace tiled
