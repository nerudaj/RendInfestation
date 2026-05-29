#pragma once

#include <SFML/Graphics.hpp>

struct [[nodiscard]] Face final
{
    sf::Vector3f origin;
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
            if (lhs.origin.z != rhs.origin.z)
                return lhs.origin.z < rhs.origin.z;
            return lhs.origin.y < rhs.origin.y;
        }
    };
} // namespace std
