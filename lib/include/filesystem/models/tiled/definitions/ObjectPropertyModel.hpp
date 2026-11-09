#pragma once

#include "../enums/ObjectPropertyType.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

namespace tiled
{
    using PropertyValueType = std::variant<bool, int, float, std::string>;

    struct ObjectPropertyModel
    {
        PropertyValueType value;
        std::string name = "";
        ObjectPropertyType type = ObjectPropertyType::Bool;
    };
} // namespace tiled

namespace nlohmann
{
    template<>
    struct adl_serializer<tiled::ObjectPropertyModel>
    {
        static void to_json(json& j, const tiled::ObjectPropertyModel& opt)
        {
            j["name"] = opt.name;
            j["type"] = opt.type;

            if (std::holds_alternative<bool>(opt.value))
            {
                j["value"] = std::get<bool>(opt.value);
            }
            else if (std::holds_alternative<int>(opt.value))
            {
                j["value"] = std::get<int>(opt.value);
            }
            else if (std::holds_alternative<float>(opt.value))
            {
                j["value"] = std::get<float>(opt.value);
            }
            else if (std::holds_alternative<std::string>(opt.value))
            {
                j["value"] = std::get<std::string>(opt.value);
            }
        }

        static void from_json(const json& j, tiled::ObjectPropertyModel& opt)
        {
            j["name"].get_to(opt.name);
            j["type"].get_to(opt.type);

            if (opt.type == tiled::ObjectPropertyType::Bool)
            {
                opt.value = j["value"].get<bool>();
            }
            else if (opt.type == tiled::ObjectPropertyType::Int)
            {
                opt.value = j["value"].get<int>();
            }
            else if (opt.type == tiled::ObjectPropertyType::Float)
            {
                opt.value = j["value"].get<float>();
            }
            else if (opt.type == tiled::ObjectPropertyType::String)
            {
                opt.value = j["value"].get<std::string>();
            }
        }
    };
} // namespace nlohmann
