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
    void update(const sf::Time& time, dgm::Camera& camera);

    [[nodiscard]] constexpr const float getCurrentZoom() const noexcept
    {
        return state == State::Default ? 1.f : 2.f;
    }

private:
    void updateTimers(const sf::Time& time);

    void updateState(dgm::Camera& camera);

    constexpr int computeCurrentEnemyCount(
        const SurvivalSpawnerContext& context) const noexcept
    {
        return context.enemiesSpawnedInCurrentWave
               - context.enemiesKilledInCurrentWave;
    }

private:
    enum class [[nodiscard]] State
    {
        Default,
        ZoomingOut,
        ZoomedOut,
        ZoomingIn
    };

    std::optional<SurvivalGameDirector>& director;
    sf::Time durationOfTooManyEnemiesConcurrently = sf::Time::Zero;
    sf::Time durationOfNotEnoughEnemiesConcurrently = sf::Time::Zero;
    State state = State::Default;
};
