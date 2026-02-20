#pragma once

#include "game/definitions/PhysicsBody.hpp"
#include "game/enums/ActorKind.hpp"
#include <DGM/classes/Animation.hpp>
#include <optional>

using Collider = std::variant<dgm::Circle, dgm::Rect>;

struct [[nodiscard]] Actor final
{
    ActorKind kind = ActorKind::None;
    PhysicsBody body;
    sf::Vector2f spriteOriginOffsetFromCollider;
    sf::Vector2f lookDirection = sf::Vector2f { 1.f, 0.f };
    dgm::Animation animation;
    int health = 100;
    std::optional<size_t> inventoryIdx = std::nullopt;
};
