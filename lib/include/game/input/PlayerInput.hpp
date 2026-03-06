#pragma once

#include "game/input/InputInterface.hpp"
#include "input/Input.hpp"

class PlayerInput final : public InputInterface
{
public:
    explicit PlayerInput(Input& input) noexcept : input(input) {}

public:
    [[nodiscard]] sf::Vector2f getForward() const override
    {
        return input.getForward();
    }

    [[nodiscard]] sf::Vector2f getAimDirection() const
    {
        return input.getAimDirection();
    }

    [[nodiscard]] bool isShootPressed() const
    {
        return input.isShootPressed();
    }

    [[nodiscard]] bool isSwapWeaponsPressed() const
    {
        return input.isSwapWeaponsPressed();
    }

private:
    Input& input;
};
