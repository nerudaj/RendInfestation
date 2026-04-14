#pragma once

#include "game/definitions/Components.hpp"
#include "game/definitions/GameScene.hpp"
#include "game/definitions/GameTextureAtlas.hpp"
#include "game/enums/EffectType.hpp"
#include "game/enums/ProjectileKind.hpp"
#include "game/enums/SkinType.hpp"
#include "input/Input.hpp"

class [[nodiscard]] ActorBuilder final
{
public:
    static entt::entity createPlayer(
        entt::registry& actors,
        const sf::Vector2f& spawnPosition,
        const GameTextureAtlas& atlas,
        Input& input,
        const WeaponLoadout& loadout);

    static entt::entity createNpc(
        entt::registry& actors,
        const sf::Vector2f& spawnPosition,
        const SkinType skin,
        const GameTextureAtlas& atlas);

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

    static void
    shatterProjectile(entt::registry& actors, entt::entity projectile);

    static entt::entity createDamageMarker(
        entt::registry& actors,
        const sf::Vector2f& origin,
        const float radius,
        const ProjectileInventory& inventory);

    static entt::entity createEffect(
        entt::registry& actors,
        const sf::Vector2f& origin,
        const EffectType type,
        const GameTextureAtlas& atlas,
        const float scale);

    static entt::entity createDoor(
        entt::registry& actors,
        const sf::Vector2f& position,
        const GameTextureAtlas& atlas);
};
