#pragma once

#include "types/SemanticTypes.hpp"
#include <DGM/classes/Animation.hpp>

constexpr float BASE_PROJECTILE_SPEED = 1024_px_per_second;
constexpr float BASE_PLAYER_SPEED = 192_px_per_second;
constexpr float BASE_ENEMY_SPEED = 192_px_per_second * 0.75f;
constexpr int BASE_PROJECTILE_DAMAGE = 25;
constexpr float BASE_PROJECTILE_KICKBACK = 50.f;
constexpr sf::Time BASE_WEAPON_FIRE_DELAY = sf::seconds(0.5f);
constexpr sf::Time BASE_PROJECTILE_LIFETIME = sf::seconds(50.f);
const dgm::AnimationStates NULL_ANIMATION_STATES = dgm::AnimationStates {
    { "idle",
      dgm::Clip(sf::Vector2u(1u, 1u), sf::IntRect({ 0u, 0u }, { 1u, 1u })) },
};
constexpr float BASE_EXPLOSION_RADIUS = 18.f;
constexpr int EFFECT_FPS = 8;
constexpr std::string_view HURT_ANIMATION_STATE = "idle-hurt";
