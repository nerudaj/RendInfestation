/*
All structs that contribute to Entity Component System (ECS) should be defined
here.
*/

#pragma once

#include "appstate/Game/Types.hpp"
#include "appstate/Game/enums/EntityKind.hpp"
#include "appstate/Game/enums/InteractionTriggerType.hpp"
#include "appstate/Game/enums/ProjectileTraits.hpp"
#include "appstate/Game/enums/SkinType.hpp"
#include "appstate/Game/enums/WeaponModule.hpp"
#include "appstate/Game/input/InputInterface.hpp"
#include "audio/SoundId.hpp"
#include "types/BrandedType.hpp"
#include "types/Overloads.hpp"
#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Collision.hpp>
#include <DGM/classes/Objects.hpp>
#include <array>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

struct [[nodiscard]] ColliderOptions final
{
    bool reportMeshCollisions = false;
    bool reportActorCollisions = false;
    bool nonblocking = false;
    bool disabled = false;
    bool semighost =
        false; ///< If true, doesn't collide with other semighost entities, but
               ///< does collide with non-semighost ones
};

struct [[nodiscard]] Collider final
{
    std::variant<dgm::Rect, dgm::Circle> shape;
    ColliderOptions options;

    template<class T>
        requires std::same_as<T, dgm::Rect> || std::same_as<T, dgm::Circle>
    bool collidesWith(const T& otherShape) const
    {
        return std::visit(
            overloads {
                [&](const dgm::Rect& r)
                { return dgm::Collision::basic(r, otherShape); },
                [&](const dgm::Circle& c)
                { return dgm::Collision::basic(otherShape, c); },
            },
            shape);
    }

    sf::Vector2f getPosition() const
    {
        return std::visit(
            overloads {
                [](const dgm::Rect& r) { return r.getCenter(); },
                [](const dgm::Circle& c) { return c.getPosition(); },
            },
            shape);
    }

    float getRadius() const
    {
        return std::visit(
            overloads {
                [](const dgm::Rect& r) { return r.getSize().length(); },
                [](const dgm::Circle& c) { return c.getRadius(); },
            },
            shape);
    }

    void move(const sf::Vector2f& vec)
    {
        std::visit(
            overloads {
                [&vec](auto& b) { b.move(vec); },
            },
            shape);
    }
};

struct [[nodiscard]] PhysicsBody final
{
    sf::Vector2f forward = { 0.f, 0.f };
    float maxSpeed = 0.f;
    float bounciness = 0.f;
    float friction = 0.5f;
    bool useAltMesh = false;
    bool canFall = true;
    // prevents controllers from using the object
    // used in Push weapon module
    bool ragdoll = false;
};

struct [[nodiscard]] Skin final
{
    EntityKind kind = EntityKind::Player;
    SkinType skinType = SkinType::PlayerDefault;
    dgm::Animation animation;
    sf::Vector2f spriteOriginOffsetFromCollider;
    float scale = 1.f;
};

using LookDirection = BrandedType<sf::Vector2f, struct LookDirectionTag>;

using Health = BrandedType<int, struct HealthTag>;

using Lifetime = BrandedType<sf::Time, struct LifetimeTag>;

using EntityInput = std::unique_ptr<InputInterface>;

using ZIndex = BrandedType<int, struct ZIndexTag>;

struct [[nodiscard]] Interval final
{
    sf::Time timer = sf::Time::Zero;
    sf::Time delay = sf::Time::Zero;
};

struct [[nodiscard]] ParticleEmitter final
{
    sf::Time emissionInterval = sf::Time::Zero;
    sf::Time emissionTimer = sf::Time::Zero;
    int particlesToEmit = 0;
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::Angle directionVariance;
    std::array<sf::Color, 2u> colors;
    float maxProjectileSize;
    float speed;
    float speedVariance;
};

struct [[nodiscard]] Particle final
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f size;
    sf::Color color;
};

struct [[nodiscard]] ParticleSystem final
{
    std::vector<Particle> particles;
};

// ===========
// INVENTORIES
// ===========
struct [[nodiscard]] BoundLightEmitter final
{
    sf::Color color;
    unsigned lightSpriteId;
};

struct [[nodiscard]] DelayedOnLeaveTriggerInventory final
{
    sf::Time delay = sf::Time::Zero;
    entt::entity targetEntity;
};

struct [[nodiscard]] InteractionTriggerInventory final
{
    InteractionTriggerType type = InteractionTriggerType::Workbench;
};

struct [[nodiscard]] DamageMarkerInventory final
{
    EntityKind originator = {};
    int damage = {};
    sf::Vector2f impactForceImpulse = {};
};

struct [[nodiscard]] ProjectileInventory final
{
    int damage = 0;
    ProjectileTraits traits = ProjectileTraits::None;
    EntityKind originator = {};
    // This is just solely for the Turret to know what weapon it should
    // get
    std::vector<WeaponModule> spawnerDef;
};

struct [[nodiscard]] Weapon final
{
    SoundId::IdType soundId = SoundId::Bullet;
    sf::Time cooldown = sf::seconds(0.1f);
    sf::Time timeTillFire = sf::seconds(0.f);
    sf::Time projectileLifetime = sf::seconds(0.f);
    float kickback = 0.f;
    float projectileSpeed = 0.f;
    float projectileSpeedVariance = 0.f;
    int spread = 1; // degrees
    int numShots = 1;
    SkinType projectileSkin = SkinType::SmallBullet;
    ProjectileInventory defaultProjectileInventory;
};

struct [[nodiscard]] WeaponInventory final
{
    int activeWeapon = 0;
    std::vector<Weapon> weapons;

    void swapWeapon()
    {
        activeWeapon = (activeWeapon + 1) % weapons.size();
    }
};
