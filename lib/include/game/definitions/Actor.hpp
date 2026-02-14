#pragma once

#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Objects.hpp>

using Collider = std::variant<dgm::Circle, dgm::Rect>;

enum class [[nodiscard]] ActorKind
{
    None,
    Prop,
    Player,
    Npc
};

struct [[nodiscard]] Actor final
{
    ActorKind kind = ActorKind::None;
    Collider body;
    sf::Vector2f forward = { 0.f, 0.f };
    sf::Angle orientation = sf::degrees(0);
    dgm::Animation animation;
};
