#pragma once

#include "types/Observable.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <nlohmann/json.hpp>
#include <optional>

struct [[nodiscard]] VideoSettingsStorageModel final
{
    sf::Vector2u resolution =
#ifdef ANDROID
        sf::VideoMode::getDesktopMode().size;
#elif _DEBUG
        sf::Vector2u(1280, 720);
#else
        sf::VideoMode::getDesktopMode().size;
#endif
    bool fullscreen =
#ifdef _DEBUG
        false;
#else
        true;
#endif
    float uiScale = 1.f;
    bool renderColliders = false;
    int fpsLimit = 3;
};

namespace sf
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector2u, x, y);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    VideoSettingsStorageModel,
    resolution,
    fullscreen,
    uiScale,
    renderColliders,
    fpsLimit);

struct [[nodiscard]] VideoSettings final
{
    UnsafeObservable<sf::Vector2u> resolution;
    bool fullscreen;
    Observable<float> uiScale;
    bool renderColliders;
    Observable<int> fpsLimit;
};
