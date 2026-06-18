#pragma once

#include "appstate/Game/builders/WeaponBuilder.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/NpcKind.hpp"
#include "appstate/Game/enums/SkinType.hpp"
#include <map>

struct [[nodiscard]] EnemyConfig final
{
    int health = 1;
    float speed = BASE_ENEMY_SPEED;
    float colliderRadius = 8.f;
    sf::Vector2f spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, 0.f };
    bool canFall = true;
    std::vector<Weapon> weapons;
    int semighostFlags = SEMIGHOST_NPC;
    NpcKind npcKind = {};
};

const std::map<SkinType, EnemyConfig> ENEMY_CONFIGS = {
    {
        SkinType::Scuttlebug,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 4,
            .speed = BASE_ENEMY_SPEED * 1.5f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            .weapons = { WeaponBuilder::createEnemyWeapon(
                BASE_MELEE_DAMAGE, EnemyWeaponKind::Slash) },
            .npcKind = NpcKind::Scuttlebug,
        },
    },
    {
        SkinType::ScuttlebugBlue,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 6,
            .speed = BASE_ENEMY_SPEED * 1.1f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            .weapons = { WeaponBuilder::createEnemyWeapon(
                BASE_MELEE_DAMAGE * 1.5f, EnemyWeaponKind::Slash) },
            .npcKind = NpcKind::GreaterScuttlebug,
        },
    },
    {
        SkinType::Bighead,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 8,
            .speed = BASE_ENEMY_SPEED * 0.8f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            .weapons = { WeaponBuilder::createEnemyWeapon(
                             BASE_MELEE_DAMAGE * 2.f,
                             EnemyWeaponKind::Headbutt),
                         WeaponBuilder::createEnemyWeapon(
                             BASE_RANGED_DAMAGE, EnemyWeaponKind::BounceBall) },
            .npcKind = NpcKind::Bighead,
        },
    },
    {
        SkinType::Beholder,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 16,
            .speed = BASE_ENEMY_SPEED * 0.6f,
            .colliderRadius = 10.f,
            .canFall = false,
            .weapons = { WeaponBuilder::createEnemyWeapon(
                BASE_RANGED_DAMAGE * 1.5f, EnemyWeaponKind::Fireball) },
            .semighostFlags = SEMIGHOST_NPC | SEMIGHOST_FLYING_STUFF,
            .npcKind = NpcKind::Beholder,
        },
    }
};
