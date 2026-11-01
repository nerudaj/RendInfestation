#include "appstate/Game/definitions/CameraZoomHandler.hpp"
#include "appstate/Game/definitions/Constants.hpp"

void CameraZoomHandler::update(const sf::Time& time, dgm::Camera& camera)
{
    updateTimers(time);
    updateState(camera);
}

void CameraZoomHandler::updateTimers(const sf::Time& time)
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
    else if (currentEnemyCount < ONLY_FEW_ENEMIES_THRESHOLD)
    {
        durationOfNotEnoughEnemiesConcurrently += time;
        if (durationOfTooManyEnemiesConcurrently > sf::Time::Zero)
        {
            durationOfTooManyEnemiesConcurrently = sf::Time::Zero;
        }
    }
}

void CameraZoomHandler::updateState(dgm::Camera& camera)
{
    switch (state)
    {
    case State::Default:
        assert(!camera.isZooming());
        if (durationOfTooManyEnemiesConcurrently >= ZOOM_OUT_TIME_THRESHOLD)
        {
            state = State::ZoomingOut;
            camera.zoomGradually(
                MAX_ZOOM_OUT_FACTOR, ZOOM_DURATION, std::identity {});
        }
        break;

    case State::ZoomingOut:
        if (!camera.isZooming()) state = State::ZoomedOut;
        break;

    case State::ZoomedOut:
        assert(!camera.isZooming());
        if (durationOfNotEnoughEnemiesConcurrently >= ZOOM_IN_TIME_THRESHOLD)
        {
            state = State::ZoomingIn;
            camera.zoomGradually(1.f, ZOOM_DURATION, std::identity {});
        }
        break;

    case State::ZoomingIn:
        if (!camera.isZooming()) state = State::Default;
        break;
    }
}
