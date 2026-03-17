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
        if (!isWalking()) return { 0.f, 0.f };

        const auto toPlayer = getDirectionToPlayer();
        if (toPlayer.length() > 20.f)
        {
            return dgm::Math::toUnit(toPlayer);
        }
        return sf::Vector2f { 0.f, 0.f };
    }

    [[nodiscard]] sf::Vector2f getAimDirection() const
    {
        return dgm::Math::toUnit(getDirectionToPlayer());
    }

    [[nodiscard]] bool isShootPressed() const
    {
        const auto toPlayer = getDirectionToPlayer();
        if (isWalking() && toPlayer.length() <= 20.f) return true;
        return false;
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

    bool isWalking() const
    {
        return scene.actors.get<Skin>(thisNpc).animation.getStateName()
               == "walk-front";
    }

private:
    const GameScene& scene;
    entt::entity thisNpc;
};
