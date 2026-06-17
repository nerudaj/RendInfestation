#pragma once

#include "types/SemanticTypes.hpp"
#include <DGM/classes/Animation.hpp>

constexpr float ACTOR_FRICTION = 0.8f;
constexpr float RAGDOLL_FRICTION = 0.1f;
constexpr float BASE_PLAYER_SPEED = 128_px_per_second;
constexpr float BASE_ENEMY_SPEED = BASE_PLAYER_SPEED * 0.5f;
constexpr float BASE_PROJECTILE_SPEED = BASE_PLAYER_SPEED * 4.f;
constexpr float BASE_PROJECTILE_PUSH_IMPACT = 356.f;
constexpr int BASE_PROJECTILE_DAMAGE = 25;
constexpr int BASE_MELEE_DAMAGE = 4;
constexpr int BASE_RANGED_DAMAGE = 8;
constexpr float BASE_PROJECTILE_KICKBACK = 50.f;
constexpr sf::Time BASE_WEAPON_FIRE_DELAY = sf::seconds(0.5f);
constexpr sf::Time BASE_PROJECTILE_LIFETIME = sf::seconds(3.f);
constexpr sf::Time EXTENDED_PROJECTILE_LIFETIME = sf::seconds(30.f);
constexpr float BASE_EXPLOSION_RADIUS = 18.f;
constexpr int EFFECT_FPS = 8;
constexpr int BASE_ANIMATION_FPS = 8;
constexpr float MIN_ENEMY_SPAWN_DISTANCE = 256.f;
constexpr int BASE_TURRET_HEALTH = BASE_PROJECTILE_DAMAGE * 10;
constexpr int BASE_PLAYER_HEALTH = 100;
constexpr int SEMIGHOST_PLAYER = 1;
constexpr int SEMIGHOST_NPC = 2;
constexpr int SEMIGHOST_PROJECTILE = 4;

// Animation states
constexpr const char* IDLE_ANIMATION_STATE = "idle";
constexpr const char* WALK_ANIMATION_STATE = "walk";
constexpr const char* HURT_ANIMATION_STATE = "idle-hurt";
constexpr const char* ATTACK_WINDUP_ANIMATION_STATE = "attack-windup";
constexpr const char* ATTACK_ANIMATION_STATE = "attack";
constexpr const char* ATTACK_RECOVERY_ANIMATION_STATE = "attack-recovery";
constexpr const char* DEATH_ANIMATION_STATE = "death";
constexpr sf::Time BASE_DOOR_CLOSE_DELAY = sf::seconds(1.f);
constexpr const char* DOOR_CLOSED_ANIMATION_STATE = "door-idle-unlocked";
constexpr const char* DOOR_OPEN_ANIMATION_STATE = "door-open";
constexpr const char* DOOR_OPENING_ANIMATION_STATE = "door-opening";
constexpr const char* DOOR_CLOSING_ANIMATION_STATE = "door-closing";

// Colors
constexpr const sf::Color COLOR_WHITE = sf::Color { 0xff, 0xf1, 0xe8 };
constexpr const sf::Color COLOR_GREEN = sf::Color { 0xa8, 0xe7, 0x2e };
constexpr const sf::Color COLOR_YELLOW = sf::Color { 0xff, 0xec, 0x27 };
constexpr const sf::Color COLOR_MUTED_YELLOW = sf::Color { 0xf3, 0xef, 0x7d };
constexpr const sf::Color COLOR_PURPLE = sf::Color { 0x83, 0x76, 0x9c };
constexpr const sf::Color COLOR_RED = sf::Color { 0xff, 0x00, 0x4d };
constexpr const sf::Color COLOR_DARK_RED = sf::Color { 0x7e, 0x25, 0x53 };
constexpr const sf::Color COLOR_DARK_GREEN = sf::Color { 0x00, 0x87, 0x51 };
constexpr const sf::Color COLOR_ORANGE = sf::Color { 0xff, 0xa3, 0x00 };
constexpr const sf::Color COLOR_BROWN = sf::Color { 0xab, 0x52, 0x36 };
constexpr const sf::Color COLOR_PINK = sf::Color { 0xff, 0x77, 0xa8 };
constexpr const sf::Color COLOR_LIGHT_GREY = sf::Color { 0xc2, 0xc3, 0xc7 };
constexpr const sf::Color COLOR_DARK_GREY = sf::Color { 0x5f, 0x57, 0x4f };

// Other
constexpr const sf::Time SPAWNER_SPAWN_DELAY = sf::seconds(0.3f);
const static inline auto INTERNAL_GAME_RESOLUTION =
    sf::Vector2f { 576.f, 324.f } / 1.5f;
constexpr float CAMERA_SHAKE_MAGNITUDE = 8.f;
constexpr float CAMERA_MOVE_SPEED = 256.f;
constexpr float FALL_SPEED = 0.75f; // percent per second

constexpr const char* FONT_NAME = "PublicPixel-rv0pA.ttf";
constexpr const unsigned FONT_BASE_HEIGHT = 8u;

// Z-indices
constexpr const int ZINDEX_FLOOR_DECOR = 0;
constexpr const int ZINDEX_COMMON = 1;
constexpr const int ZINDEX_PROJECTILES = 100;
constexpr const int ZINDEX_FOREGROUND_DECOR = 200;
