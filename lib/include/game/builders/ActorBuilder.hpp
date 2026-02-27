#pragma once

#include "game/definitions/Actor.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "game/enums/EffectType.hpp"

class [[nodiscard]] ActorBuilder final
{
public:
    static Actor createPlayer(
        const sf::Vector2f& spawnPosition,
        const GameTextureAtlas& atlas,
        size_t inventoryIdx);

    static Actor createNpc(
        const sf::Vector2f& spawnPosition,
        const GameTextureAtlas& atlas,
        size_t inventoryIdx);

    static Actor createProjectile(
        const sf::Vector2f& origin,
        const sf::Vector2f& direction,
        const GameTextureAtlas& atlas,
        size_t inventoryIdx);

    static Actor createEffect(
        const sf::Vector2f& origin,
        const EffectType type,
        const GameTextureAtlas& atlas);

    static Actor createProp(
        const sf::Vector2f& origin,
        size_t propId,
        const GameTextureAtlas& atlas);

    static Actor createDamageMarker(
        const sf::Vector2f& origin, const float radius, size_t inventoryIdx);
};
