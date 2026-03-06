#pragma once

#include <SFML/System/Vector2.hpp>

class [[nodiscard]] InputInterface
{
public:
    virtual ~InputInterface() = default;

public:
    virtual [[nodiscard]] sf::Vector2f getForward() const = 0;

    virtual [[nodiscard]] sf::Vector2f getAimDirection() const = 0;

    virtual [[nodiscard]] bool isShootPressed() const = 0;

    virtual [[nodiscard]] bool isSwapWeaponsPressed() const = 0;
};
