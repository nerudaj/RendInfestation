#pragma once

#include <SFML/Graphics.hpp>

struct [[nodiscard]] SortCoords final
{
    float y;
    float z;
};

struct [[nodiscard]] Face final
{
    sf::Vector2f origin;
    SortCoords sort;
    sf::FloatRect texUvs;
    sf::Vector2f scale = { 1.f, 1.f };
    sf::Angle rotation = sf::degrees(0);
};

namespace std
{
    template<>
    struct less<Face>
    {
        bool operator()(const Face& lhs, const Face& rhs) const noexcept
        {
            if (lhs.sort.z != rhs.sort.z) return lhs.sort.z < rhs.sort.z;
            return lhs.sort.y < rhs.sort.y;
        }
    };
} // namespace std
