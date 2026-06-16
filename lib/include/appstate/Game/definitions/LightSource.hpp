#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>

struct [[nodiscard]] LightSource final
{
    sf::Vector2f position;
    size_t spriteId;
    sf::Color color;
};
