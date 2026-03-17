#pragma once

#include "game/definitions/GameScene.hpp"
#include "game/input/InputInterface.hpp"

class NpcInput final : public InputInterface
{
public:
    explicit NpcInput(const GameScene& scene, entt::entity thisNpc) noexcept
        : scene(scene), thisNpc(thisNpc)
    {
    }

public:
    [[nodiscard]] sf::Vector2f getForward() const override
    {
        const auto toPlayer = getDirectionToPlayer();
        if (toPlayer.length() > 20.f)
        {
            return dgm::Math::toUnit(toPlayer);
        }
    }

    [[nodiscard]] sf::Vector2f getAimDirection() const
    {
        return dgm::Math::toUnit(getDirectionToPlayer());
    }

    [[nodiscard]] bool isShootPressed() const
    {
        const auto toPlayer = getDirectionToPlayer();
        if (scene.actors.get<Skin>(thisNpc).animation.getStateName()
                == "walk-front"
            && toPlayer.length() <= 20.f)
            return true;
    }

    [[nodiscard]] bool isSwapWeaponsPressed() const
    {
        return false;
    }

private:
    sf::Vector2f getDirectionToPlayer() const
    {
        return scene.actors.get<Collider>(scene.playerEntity).getPosition()
               - scene.actors.get<Collider>(thisNpc).getPosition();
    }

private:
    const GameScene& scene;
    entt::entity thisNpc;
};
