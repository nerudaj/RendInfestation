#pragma once

#include "game/Types.hpp"
#include "game/definitions/PhysicsBody.hpp"
#include "game/definitions/Weapon.hpp"
#include "game/enums/ActorKind.hpp"
#include "game/enums/ActorSkin.hpp"
#include "game/input/InputInterface.hpp"
#include "types/BrandedType.hpp"
#include <DGM/classes/Animation.hpp>
#include <memory>
#include <optional>
#include <vector>

struct [[nodiscard]] Skin final
{
    ActorKind kind = ActorKind::Player;
    SkinType skinType = SkinType::PlayerDefault;
    dgm::Animation animation;
    sf::Vector2f spriteOriginOffsetFromCollider;
};

using LookDirection = BrandedType<sf::Vector2f, struct LookDirectionTag>;

using Health = BrandedType<int, struct HealthTag>;

struct [[nodiscard]] WeaponInventory final
{
    int activeWeapon = 0;
    std::vector<Weapon> weapons;
};

using Lifetime = BrandedType<sf::Time, struct LifetimeTag>;

using EntityInput = std::unique_ptr<InputInterface>;

struct [[nodiscard]] BoundLightEmitter final
{
    sf::Color color;
    size_t lightSpriteId;
};
