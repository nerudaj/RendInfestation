#pragma once

#include <SFML/System/Vector2.hpp>

class [[nodiscard]] InputInterface
{
public:
    virtual ~InputInterface() = default;

public:
    [[nodiscard]] virtual sf::Vector2f getForward() const = 0;

    [[nodiscard]] virtual sf::Vector2f getAimDirection() const = 0;

    [[nodiscard]] virtual bool isShootPressed() const = 0;

    [[nodiscard]] virtual bool isSwapWeaponsPressed() const = 0;

    [[nodiscard]] virtual bool isInteractPressed() const = 0;

    [[nodiscard]] virtual void freeze() = 0;
};
