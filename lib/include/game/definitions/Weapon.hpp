#pragma once

struct [[nodiscard]] Weapon final
{
    const sf::Time cooldown = sf::seconds(0.1f);
    sf::Time timeTillFire = sf::seconds(0.f);
    float kickback = 100.f;
    int spread = 2; // degrees
    int numShots = 1;
};
