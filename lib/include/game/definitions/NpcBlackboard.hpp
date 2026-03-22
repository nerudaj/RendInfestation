#pragma once

#include <SFML/System/Vector2.hpp>

struct [[nodiscard]] NpcBlackboard final
{
    entt::entity targetEntity = entt::null;
    sf::Vector2f movementTarget;
};
