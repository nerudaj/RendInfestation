#pragma once

#include "appstate/Game/definitions/Components.hpp"
#include <SFML/System/Vector2.hpp>
#include <array>
#include <functional>
#include <optional>
#include <string>

struct [[nodiscard]] Prop final
{
    std::string animationStateName;
    std::function<Collider(const sf::Vector2f&)> getCollider;
    sf::Vector2f spriteOffset = { 0.f, 0.f };
    bool isSolid = true;
    std::optional<BoundLightEmitter> boundLightEmitter = std::nullopt;
};

const std::array<Prop, 13u> PROP_DEFINITIONS = {
    Prop {
        .animationStateName = "labtube-full",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Circle({ origin.x + 32.f, origin.y - 16.f }, 13.f),
            };
        },
        .spriteOffset = { 0.f, -16.f },
        .boundLightEmitter = BoundLightEmitter { COLOR_GREEN, 7 },
    },
    Prop {
        .animationStateName = "labtube",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Circle({ origin.x + 32.f, origin.y - 16.f }, 13.f),
            };
        },
        .spriteOffset = { 0.f, -16.f },
        .boundLightEmitter = BoundLightEmitter { COLOR_GREEN, 7 },
    },
    Prop {
        .animationStateName = "small-table",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }) };
        },
    },
    Prop {
        .animationStateName = "cantina-table",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 64.f }, { 64.f, 56.f }) };
        },
        .spriteOffset = { 0.f, -4.f },
    },
    Prop {
        .animationStateName = "green-carcass",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 32.f, 32.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 16.f, 16.f },
        .isSolid = false,
    },
    Prop {
        .animationStateName = "blue-carcass",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 32.f, 32.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 16.f, 16.f },
        .isSolid = false,
    },
    Prop {
        .animationStateName = "blood-puddle-a",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 24.f, 14.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 22.f, 25.f },
        .isSolid = false,
    },
    Prop {
        .animationStateName = "pc",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 20.f, 14.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 22.f, 25.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "pc-active",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 20.f, 14.f }),
                ColliderOptions { .nonblocking = true, .disabled = true }
            };
        },
        .spriteOffset = { 22.f, 25.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "cactus-pot",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 56.f }, { 15.f, 10.f }) };
        },
        .spriteOffset = { 25.f, 18.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "cactus-pot-destroyed",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 56.f }, { 15.f, 10.f }) };
        },
        .spriteOffset = { 25.f, 18.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "crate",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x, origin.y - 64.f }, { 11.f, 10.f }) };
        },
        .spriteOffset = { 26.f, 23.f },
        .isSolid = true,
    },
    Prop {
        .animationStateName = "workbench",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider { dgm::Rect(
                { origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }) };
        },
    },
};
