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
    sf::Angle orientation = sf::degrees(0);
    dgm::Animation animation;
    int health = 100;
    std::optional<size_t> inventoryIdx = std::nullopt;
};
