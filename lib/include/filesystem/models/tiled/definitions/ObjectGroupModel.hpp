#pragma once

#include "../enums/DrawOrder.hpp"
#include "../enums/LayerType.hpp"
#include "ObjectModel.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tiled
{
    struct ObjectGroupModel
    {
        int id = 0;
        int x = 0;
        int y = 0;
        std::string name;
        std::vector<ObjectModel> objects = {};
        DrawOrder draworder = DrawOrder::Uninitialized;
        LayerType type = LayerType::Uninitialized;
        bool visible = true;
        unsigned opacity = 0;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        ObjectGroupModel,
        id,
        x,
        y,
        name,
        objects,
        draworder,
        type,
        visible,
        opacity);
} // namespace tiled
