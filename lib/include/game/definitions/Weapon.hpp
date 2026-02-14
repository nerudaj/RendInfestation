#pragma once

struct [[nodiscard]] Weapon final
{
    const sf::Time cooldown = sf::seconds(0.5f);
    sf::Time timeTillFire = sf::seconds(0.f);
    float kickback = 0.f;
};
