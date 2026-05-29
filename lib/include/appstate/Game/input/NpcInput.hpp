#pragma once

#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/input/InputInterface.hpp"

class NpcInput final : public InputInterface
{
public:
    void setForward(const sf::Vector2f& newForward)
    {
        if (frozen) return;
        forward = newForward;
    }

    [[nodiscard]] sf::Vector2f getForward() const override
    {
        return forward;
    }

    void setAimDirection(const sf::Vector2f& newAimDirection)
    {
        aimDirection = newAimDirection;
    }

    [[nodiscard]] sf::Vector2f getAimDirection() const override
    {
        return aimDirection;
    }

    void setShooting(bool value)
    {
        shooting = value;
    }

    [[nodiscard]] bool isShootPressed() const override
    {
        return shooting;
    }

    [[nodiscard]] bool isSwapWeaponsPressed() const override
    {
        return false;
    }

    [[nodiscard]] bool isInteractPressed() const override
    {
        return false;
    }

    void freeze() override
    {
        frozen = true;
    }

    void clearInputs()
    {
        forward = {};
        aimDirection = {};
        shooting = false;
    }

private:
    sf::Vector2f forward = {};
    sf::Vector2f aimDirection = {};
    bool shooting = false;
    bool frozen = false;
};
