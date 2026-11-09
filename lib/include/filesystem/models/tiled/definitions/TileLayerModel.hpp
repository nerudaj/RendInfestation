#pragma once

#include "../enums/LayerType.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tiled
{
    struct TileLayerModel
    {
        unsigned id = 0;
        std::string name = "";
        LayerType type = LayerType::Uninitialized;
        std::vector<int> data = {};
        int x = 0;
        int y = 0;
        unsigned width = 0;
        unsigned height = 0;
        unsigned opacity = 0;
        bool visible = true;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        TileLayerModel,
        id,
        name,
        data,
        x,
        y,
        width,
        height,
        opacity,
        type,
        visible);
} // namespace tiled
