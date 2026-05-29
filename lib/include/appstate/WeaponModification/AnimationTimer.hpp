#pragma once

#include <SFML/System/Time.hpp>

class [[nodiscard]] AnimationTimer final
{
public:
    explicit AnimationTimer(sf::Time duration)
    : duration(duration)
    {}

public:
    bool isFinished() const noexcept { return timer == sf::Time::Zero; }

    void update(const sf::Time elapsed)
    {
        if (timer == sf::Time::Zero)
            return;

        timer -= elapsed;
        if (timer < sf::Time::Zero)
            timer = sf::Time::Zero;
    }

    void restart()
    {
        timer = duration;
    }

    float getFactor() const
    {
        return 1.f - timer / duration;
    }

private:
    sf::Time duration;
    sf::Time timer = sf::Time::Zero;
};