#pragma once

#include "misc/Compatibility.hpp"
#include <SFML/System/Time.hpp>
#include <chrono>
#include <string>

class [[nodiscard]] TimeFormatter final
{
public:
    static std::string toMMSS(const sf::Time& time)
    {
        const auto seconds =
            std::chrono::duration_cast<std::chrono::seconds>(time.toDuration());
        const auto minutes =
            std::chrono::duration_cast<std::chrono::minutes>(seconds);
        return uni::format(
            "{:02}:{:02}", minutes.count(), (seconds - minutes).count());
    }
};
