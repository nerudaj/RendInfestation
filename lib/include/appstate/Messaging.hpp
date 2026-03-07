#pragma once

#include <optional>
#include <string>
#include <typeinfo>
#include <variant>

struct [[nodiscard]] PopIfNotMenu final
{
};

struct [[nodiscard]] PopIfNotGame final
{
};

using AppMessage = std::variant<PopIfNotMenu, PopIfNotGame>;

template<class T>
concept IsAppMessage = std::constructible_from<AppMessage, T>;

class Messaging
{
public:
    template<IsAppMessage T>
    static const char* serialize()
    {
        return typeid(T).name();
    }

    static std::optional<AppMessage> deserialize(const std::string& str);
};
