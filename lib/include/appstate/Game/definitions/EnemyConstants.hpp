#pragma once

#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/SkinType.hpp"
#include <map>

struct [[nodiscard]] EnemyConfig final
{
    int health = 1;
    int damage = 1;
    float speed = BASE_ENEMY_SPEED;
    float colliderRadius = 8.f;
    sf::Vector2f spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, 0.f };
    bool canFall = true;
    bool isRanged = false;
};

const std::map<SkinType, EnemyConfig> ENEMY_CONFIGS = {
    {
        SkinType::Scuttlebug,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 4,
            .damage = BASE_MELEE_DAMAGE,
            .speed = BASE_ENEMY_SPEED * 1.5f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
        },
    },
    {
        SkinType::ScuttlebugBlue,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 6,
            .damage = static_cast<int>(BASE_MELEE_DAMAGE * 1.5f),
            .speed = BASE_ENEMY_SPEED * 1.1f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
        },
    },
    {
        SkinType::Bighead,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 8,
            .damage = BASE_MELEE_DAMAGE * 2,
            .speed = BASE_ENEMY_SPEED * 0.8f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
        },
    },
    {
        SkinType::Beholder,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 16,
            .damage = BASE_RANGED_DAMAGE,
            .speed = BASE_ENEMY_SPEED * 0.6f,
            .colliderRadius = 10.f,
            .canFall = false,
            .isRanged = true,
        },
    }
};
