#pragma once

#include <SFML/System/Time.hpp>

class [[nodiscard]] Blinker final
{
public:
    explicit Blinker(const sf::Time period = sf::seconds(0.15f))
        : period(period)
    {
    }

public:
    void update(const sf::Time& elapsed)
    {
        timer -= elapsed;
        if (timer < sf::Time::Zero)
        {
            timer += period;
            on = !on;
        }
    }

    [[nodiscard]] constexpr bool isOn() const noexcept
    {
        return on;
    }

private:
    bool on = true;
    const sf::Time period;
    sf::Time timer = sf::seconds(0.25f);
};
