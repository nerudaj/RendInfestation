#pragma once

#include <SFML/Graphics.hpp>

struct [[nodiscard]] Face final
{
    sf::Vector2f origin;
    sf::FloatRect texUvs;
    sf::Vector2f scale = { 1.f, 1.f };
};

namespace std
{
    template<>
    struct less<Face>
    {
        bool operator()(const Face& lhs, const Face& rhs) const noexcept
        {
            return lhs.origin.y < rhs.origin.y;
        }
    };
} // namespace std
