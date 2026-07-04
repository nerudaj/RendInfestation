#pragma once

#include "appstate/Game/builders/WeaponBuilder.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/NpcKind.hpp"
#include "appstate/Game/enums/SkinType.hpp"
#include <map>

struct [[nodiscard]] EnemyConfig final
{
    int health = 1;
    SoundId::IdType deathSound = SoundId::Error;
    float speed = BASE_ENEMY_SPEED;
    float colliderRadius = 8.f;
    sf::Vector2f spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, 0.f };
    bool canFall = true;
    std::vector<Weapon> weapons;
    int semighostFlags = SEMIGHOST_NPC;
    NpcKind npcKind = {};
    ZIndex zindex = ZIndex(ZINDEX_COMMON);
};

const std::map<SkinType, EnemyConfig> ENEMY_CONFIGS = {
    {
        SkinType::Scuttlebug,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 4,
            .deathSound = SoundId::ScuttlebugDeath,
            .speed = BASE_ENEMY_SPEED * 1.5f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            .weapons = { WeaponBuilder::createEnemyWeapon(
                BASE_MELEE_DAMAGE, EnemyWeaponKind::Slash) },
            .semighostFlags = SEMIGHOST_NPC | SEMIGHOST_CRAWLER,
            .npcKind = NpcKind::Scuttlebug,
            .zindex = ZIndex(ZINDEX_LITTLE),
        },
    },
    {
        SkinType::ScuttlebugBlue,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 6,
            .deathSound = SoundId::ScuttlebugDeath,
            .speed = BASE_ENEMY_SPEED * 1.1f,
            .spriteOriginOffsetFromCollider = sf::Vector2f { 0.f, -10.f },
            .weapons = { WeaponBuilder::createEnemyWeapon(
                BASE_MELEE_DAMAGE * 1.5f, EnemyWeaponKind::Slash) },
            .semighostFlags = SEMIGHOST_NPC | SEMIGHOST_CRAWLER,
            .npcKind = NpcKind::GreaterScuttlebug,
            .zindex = ZIndex(ZINDEX_LITTLE),
        },
    },
    {
        SkinType::Bighead,
        EnemyConfig {
            .health = BASE_PROJECTILE_DAMAGE * 8,
            .deathSound = SoundId::BigheadDeath,
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
            .deathSound = SoundId::BeholderDeath,
            .speed = BASE_ENEMY_SPEED * 0.6f,
            .colliderRadius = 10.f,
            .canFall = false,
            .weapons = { WeaponBuilder::createEnemyWeapon(
                BASE_RANGED_DAMAGE * 1.5f, EnemyWeaponKind::Fireball) },
            .semighostFlags = SEMIGHOST_NPC | SEMIGHOST_FLYING_STUFF,
            .npcKind = NpcKind::Beholder,
            .zindex = ZIndex(ZINDEX_FLYING),
        },
    }
};
