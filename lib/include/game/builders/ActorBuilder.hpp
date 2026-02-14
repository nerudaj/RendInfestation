#pragma once

#include "game/definitions/Actor.hpp"
#include "game/definitions/GameTextureAtlas.hpp"

class [[nodiscard]] ActorBuilder final
{
public:
    static Actor createProjectile(
        const sf::Vector2f& origin,
        const sf::Vector2f& direction,
        const GameTextureAtlas& atlas,
        size_t inventoryIdx);
};
