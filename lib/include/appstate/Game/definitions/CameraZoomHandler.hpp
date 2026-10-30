#pragma once

#include "appstate/Game/SurvivalGameDirector.hpp"
#include <DGM/classes/Camera.hpp>
#include <SFML/System/Time.hpp>
#include <optional>

class [[nodiscard]] CameraZoomHandler final
{
public:
    explicit CameraZoomHandler(
        std::optional<SurvivalGameDirector>& director) noexcept
        : director(director)
    {
    }

public:
    void update(const sf::Time& time);

    void triggerCameraTransitions(dgm::Camera& camera);

    [[nodiscard]] constexpr const float getCurrentZoom() const noexcept
    {
        return currentZoom;
    }

private:
    constexpr int computeCurrentEnemyCount(
        const SurvivalSpawnerContext& context) const noexcept
    {
        return context.enemiesSpawnedInCurrentWave
               - context.enemiesKilledInCurrentWave;
    }

private:
    std::optional<SurvivalGameDirector>& director;
    sf::Time durationOfTooManyEnemiesConcurrently = sf::Time::Zero;
    sf::Time durationOfNotEnoughEnemiesConcurrently = sf::Time::Zero;
    float currentZoom = 1.f;
};
