#pragma once

#include "ObjectPropertyModel.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tiled
{
    struct ObjectModel
    {
        int gid = 0;
        int id = 0;
        float x = 0;
        float y = 0;
        unsigned width = 0;
        unsigned height = 0;
        std::string name;
        std::string type;
        int rotation = 0;
        bool point = true;
        bool visible = true;
        std::vector<ObjectPropertyModel> properties;

        friend void to_json(nlohmann::json& j, const ObjectModel& model)
        {
            j["gid"] = model.gid;
            j["id"] = model.id;
            j["x"] = model.x;
            j["y"] = model.y;
            j["width"] = model.width;
            j["height"] = model.height;
            j["name"] = model.name;
            j["type"] = model.type;
            j["rotation"] = model.rotation;
            j["point"] = model.point;
            j["visible"] = model.visible;
            j["properties"] = model.properties;
        }

        friend void from_json(const nlohmann::json& j, ObjectModel& model)
        {
            if (j.contains("gid")) j["gid"].get_to(model.gid);
            if (j.contains("id")) j["id"].get_to(model.id);
            if (j.contains("x")) j["x"].get_to(model.x);
            if (j.contains("y")) j["y"].get_to(model.y);
            if (j.contains("width")) j["width"].get_to(model.width);
            if (j.contains("height")) j["height"].get_to(model.height);
            if (j.contains("name")) j["name"].get_to(model.name);
            if (j.contains("type")) j["type"].get_to(model.type);
            if (j.contains("rotation")) j["rotation"].get_to(model.rotation);
            if (j.contains("point")) j["point"].get_to(model.point);
            if (j.contains("visible")) j["visible"].get_to(model.visible);
            if (j.contains("properties"))
                j["properties"].get_to(model.properties);
        }
    };
} // namespace tiled
