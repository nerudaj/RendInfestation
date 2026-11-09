#pragma once

#include "filesystem/models/tiled/definitions/ObjectGroupModel.hpp"
#include "filesystem/models/tiled/definitions/TileLayerModel.hpp"
#include "filesystem/models/tiled/definitions/TilesetModel.hpp"
#include "filesystem/models/tiled/enums/LayerType.hpp"
#include "filesystem/models/tiled/enums/MapType.hpp"
#include "filesystem/models/tiled/enums/Orientation.hpp"
#include "filesystem/models/tiled/enums/RenderOrder.hpp"
#include "filesystem/models/tiled/enums/StaggerAxis.hpp"
#include "filesystem/models/tiled/enums/StaggerIndex.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tiled
{
    // docs: https://doc.mapeditor.org/en/stable/reference/json-map-format/
    using LayerModel = std::variant<TileLayerModel, ObjectGroupModel>;

    struct FiniteMapModel
    {
        int compressionlevel = 0;
        int nextlayerid = 0;
        int nextobjectid = 0;
        unsigned width = 0;
        unsigned height = 0;
        unsigned tilewidth = 0;
        unsigned tileheight = 0;
        std::vector<LayerModel> layers;
        bool infinite = false;
        Orientation orientation = {};
        RenderOrder renderorder = {};
        std::vector<TilesetModel> tilesets = {};
        MapType type = {};
        std::string tiledversion = "";
        std::string version = "";
        double parallaxoriginx = 0.0;
        double parallaxoriginy = 0.0;
        StaggerAxis staggeraxis = {};
        StaggerIndex staggerindex = {};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        FiniteMapModel,
        compressionlevel,
        nextlayerid,
        nextobjectid,
        width,
        height,
        tilewidth,
        tileheight,
        layers,
        infinite,
        orientation,
        renderorder,
        tilesets,
        type,
        tiledversion,
        version);

} // namespace tiled

namespace nlohmann
{
    template<>
    struct adl_serializer<tiled::LayerModel>
    {
        static void to_json(json& j, const tiled::LayerModel& opt)
        {
            if (std::holds_alternative<tiled::TileLayerModel>(opt))
            {
                j = std::get<tiled::TileLayerModel>(opt);
            }
            else
            {
                j = std::get<tiled::ObjectGroupModel>(opt);
            }
        }

        static void from_json(const json& j, tiled::LayerModel& opt)
        {
            if (j.contains("type"))
            {
                tiled::LayerType type;
                j["type"].get_to(type);

                if (type == tiled::LayerType::TileLayer)
                {
                    tiled::TileLayerModel model = j;
                    opt = model;
                }
                else
                {
                    tiled::ObjectGroupModel model = j;
                    opt = model;
                }
            }
        }
    };
} // namespace nlohmann
