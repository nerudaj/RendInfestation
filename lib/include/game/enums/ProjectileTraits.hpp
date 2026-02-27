#pragma once

#include <utility>

enum class [[nodiscard]] ProjectileTraits
{
    None = 0,
    Explosive = 1,
    Bouncy = 2,
    Passthru = 4,
    Shrapnels = 8,
};

constexpr ProjectileTraits operator|(ProjectileTraits a, ProjectileTraits b)
{
    using T = std::underlying_type_t<ProjectileTraits>;
    return static_cast<ProjectileTraits>(static_cast<T>(a) | static_cast<T>(b));
}

constexpr bool operator&(ProjectileTraits base, ProjectileTraits question)
{
    using T = std::underlying_type_t<ProjectileTraits>;
    return static_cast<T>(base) & static_cast<T>(question);
}
