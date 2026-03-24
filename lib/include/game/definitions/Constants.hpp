#pragma once

#include "types/SemanticTypes.hpp"
#include <DGM/classes/Animation.hpp>

constexpr float BASE_PLAYER_SPEED = 128_px_per_second;
constexpr float BASE_ENEMY_SPEED = BASE_PLAYER_SPEED * 0.5f;
constexpr float BASE_PROJECTILE_SPEED = BASE_PLAYER_SPEED * 4.f;
constexpr int BASE_PROJECTILE_DAMAGE = 25;
constexpr int BASE_MELEE_DAMAGE = 10;
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
constexpr sf::Time BASE_DOOR_CLOSE_DELAY = sf::seconds(1.f);
constexpr const char* DOOR_CLOSED_ANIMATION_STATE = "door-idle-unlocked";
constexpr const char* DOOR_OPEN_ANIMATION_STATE = "door-open";
constexpr const char* DOOR_OPENING_ANIMATION_STATE = "door-opening";
constexpr const char* DOOR_CLOSING_ANIMATION_STATE = "door-closing";
constexpr const sf::Color COLOR_WHITE = sf::Color { 0xff, 0xf1, 0xe8 };
constexpr const sf::Color COLOR_GREEN = sf::Color { 0xa8, 0xe7, 0x2e };
constexpr const sf::Color COLOR_YELLOW = sf::Color { 0xff, 0xec, 0x27 };
constexpr const sf::Color COLOR_MUTED_YELLOW = sf::Color { 0xf3, 0xef, 0x7d };
constexpr const sf::Time SPAWNER_SPAWN_DELAY = sf::seconds(1.f);
const static inline auto INTERNAL_GAME_RESOLUTION =
    sf::Vector2f { 576.f, 324.f } / 1.5f;
constexpr float CAMERA_SHAKE_MAGNITUDE = 8.f;
constexpr float CAMERA_MOVE_SPEED = 256.f;
constexpr float FALL_SPEED = 0.75f; // percent per second
