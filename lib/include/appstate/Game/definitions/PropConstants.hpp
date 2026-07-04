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
    std::optional<Health> health = std::nullopt;
    ZIndex zIndex = ZIndex(ZINDEX_COMMON);
};

const std::array<Prop, 16u> PROP_DEFINITIONS = {
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
            return Collider {
                dgm::Rect({ origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }),
                ColliderOptions {
                    .semighost = SEMIGHOST_PROJECTILE | SEMIGHOST_FLYING_STUFF
                                 | SEMIGHOST_CRAWLER,
                }
            };
        },
    },
    Prop {
        .animationStateName = "cantina-table",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 64.f, 56.f }),
                ColliderOptions {
                    .semighost = SEMIGHOST_PROJECTILE | SEMIGHOST_FLYING_STUFF
                                 | SEMIGHOST_CRAWLER,
                }
            };
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
                ColliderOptions {
                    .nonblocking = true,
                    .disabled = true,
                }
            };
        },
        .spriteOffset = { 16.f, 16.f },
        .isSolid = false,
        .zIndex = ZIndex(ZINDEX_FLOOR_DECOR),
    },
    Prop {
        .animationStateName = "blue-carcass",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 32.f, 32.f }),
                ColliderOptions {
                    .nonblocking = true,
                    .disabled = true,
                }
            };
        },
        .spriteOffset = { 16.f, 16.f },
        .isSolid = false,
        .zIndex = ZIndex(ZINDEX_FLOOR_DECOR),
    },
    Prop {
        .animationStateName = "blood-puddle-a",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 24.f, 14.f }),
                ColliderOptions {
                    .nonblocking = true,
                    .disabled = true,
                }
            };
        },
        .spriteOffset = { 22.f, 25.f },
        .isSolid = false,
        .zIndex = ZIndex(ZINDEX_FLOOR_DECOR),
    },
    Prop {
        .animationStateName = "pc",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 20.f, 14.f }),
                ColliderOptions {
                    .nonblocking = true,
                    .disabled = true,
                }
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
                ColliderOptions {
                    .nonblocking = true,
                    .disabled = true,
                }
            };
        },
        .spriteOffset = { 22.f, 25.f },
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
        .health =
            Health {
                .value = 50,
                .deathSound = SoundId::TreeDestroyed,
            },
    },
    Prop {
        .animationStateName = "cactus-pot-destroyed",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 56.f }, { 15.f, 10.f }),
                ColliderOptions {
                    .semighost = SEMIGHOST_PROJECTILE | SEMIGHOST_FLYING_STUFF,
                }
            };
        },
        .spriteOffset = { 25.f, 18.f },
    },
    Prop {
        .animationStateName = "crate",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x, origin.y - 64.f }, { 11.f, 10.f }),
                ColliderOptions {
                    .semighost = SEMIGHOST_PROJECTILE | SEMIGHOST_FLYING_STUFF,
                }
            };
        },
        .spriteOffset = { 26.f, 23.f },
    },
    Prop {
        .animationStateName = "workbench",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x + 16.f, origin.y - 48.f }, { 32.f, 24.f }),
                ColliderOptions {
                    .semighost = SEMIGHOST_PROJECTILE | SEMIGHOST_FLYING_STUFF
                                 | SEMIGHOST_CRAWLER,
                }
            };
        },
    },
    Prop {
        .animationStateName = "tree",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x + 10.f, origin.y - 48.f }, { 4.f, 12.f }),
                ColliderOptions {},
            };
        },
        .spriteOffset = { 20.f, 10.f },
        .health =
            Health {
                .value = 75,
                .deathSound = SoundId::TreeDestroyed,
            },
    },
    Prop {
        .animationStateName = "tree-destroyed",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Rect({ origin.x + 10.f, origin.y - 48.f }, { 4.f, 12.f }),
                ColliderOptions {
                    .nonblocking = true,
                    .disabled = true,
                }
            };
        },
        .spriteOffset = { 20.f, 10.f },
        .isSolid = false,
    },
    Prop {
        .animationStateName = "plant",
        .getCollider =
            [](const sf::Vector2f& origin)
        {
            return Collider {
                dgm::Circle({ origin.x + 7.f, origin.y - 52.f }, 1.f),
                ColliderOptions {
                    .nonblocking = true,
                    .disabled = true,
                },
            };
        },
        .spriteOffset = { 25.f, 19.f },
        .isSolid = false,
        .zIndex = ZIndex(ZINDEX_LITTLE),
    },
};
