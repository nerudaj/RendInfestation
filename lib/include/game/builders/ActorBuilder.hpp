#pragma once

#include "game/definitions/Actor.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "game/definitions/Weapon.hpp"
#include "game/enums/EffectType.hpp"
#include "game/enums/ProjectileKind.hpp"
#include "input/Input.hpp"

class [[nodiscard]] ActorBuilder final
{
public:
    static entt::entity createPlayer(
        entt::registry& actors,
        const sf::Vector2f& spawnPosition,
        const GameTextureAtlas& atlas,
        Input& input);

    static entt::entity createProp(
        entt::registry& actors,
        const sf::Vector2f& origin,
        size_t propId,
        const GameTextureAtlas& atlas);

    static entt::entity createProjectile(
        entt::registry& actors,
        const sf::Vector2f& origin,
        const sf::Vector2f& direction,
        const GameTextureAtlas& atlas,
        const Weapon& weapon);

    /*static Actor createNpc(
        const sf::Vector2f& spawnPosition,
        const GameTextureAtlas& atlas,
        size_t inventoryIdx);

    static Actor createProjectile(
        const sf::Vector2f& origin,
        const sf::Vector2f& direction,
        const GameTextureAtlas& atlas,
        const Weapon& weapon,
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
        const sf::Vector2f& origin, const float radius, size_t inventoryIdx);*/
};
