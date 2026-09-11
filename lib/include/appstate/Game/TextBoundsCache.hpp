#pragma once

#include <SFML/Graphics/Text.hpp>
#include <unordered_map>

class [[nodiscard]] TextBoundsCache final
{
public:
    sf::Vector2f getGlobalBounds(const sf::Text& text);

private:
    std::unordered_map<std::string, sf::Vector2f> cache;
};
