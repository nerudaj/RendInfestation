#include "appstate/Game/definitions/CameraZoomHandler.hpp"
#include "appstate/Game/definitions/Constants.hpp"

void CameraZoomHandler::update(const sf::Time& time)
{
    if (!director) return;
    auto& context = director->getContext();

    const auto currentEnemyCount = computeCurrentEnemyCount(context);
    if (currentEnemyCount >= TOO_MANY_ENEMIES_THRESHOLD)
    {
        durationOfTooManyEnemiesConcurrently += time;
        if (durationOfNotEnoughEnemiesConcurrently > sf::Time::Zero)
        {
            durationOfNotEnoughEnemiesConcurrently = sf::Time::Zero;
        }
    }
    else if (currentEnemyCount < TOO_MANY_ENEMIES_THRESHOLD)
    {
        durationOfNotEnoughEnemiesConcurrently += time;
        if (durationOfTooManyEnemiesConcurrently > sf::Time::Zero)
        {
            durationOfTooManyEnemiesConcurrently = sf::Time::Zero;
        }
    }
}

void CameraZoomHandler::triggerCameraTransitions(dgm::Camera& camera)
{
    if (currentZoom == 1.f && !camera.isZooming()
        && durationOfTooManyEnemiesConcurrently >= ZOOM_OUT_TIME_THRESHOLD)
    {
        camera.zoomGradually(2.f, sf::seconds(4.5f), std::identity {});
        currentZoom = 2.f;
    }
    else if (
        currentZoom == 2.f && !camera.isZooming()
        && durationOfNotEnoughEnemiesConcurrently >= ZOOM_IN_TIME_THRESHOLD)
    {
        camera.zoomGradually(1.f, sf::seconds(4.5f), std::identity {});
        currentZoom = 1.f;
    }
}
