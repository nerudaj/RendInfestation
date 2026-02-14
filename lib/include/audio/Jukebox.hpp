#pragma once

#include <DGM/classes/ResourceManager.hpp>
#include <SFML/Audio/Music.hpp>
#include <optional>

class [[nodiscard]] Jukebox final
{
public:
    explicit Jukebox(dgm::ResourceManager& resmgr) noexcept : resmgr(resmgr) {}

    Jukebox(const Jukebox&) = delete;
    Jukebox(Jukebox&&) = delete;

public:
    void setVolume(float newVolume);

    void play(const std::string& trackName, bool loop = true);

    void stop();

    void resume();

private:
    sf::Music& getCurrentTrack();

private:
    dgm::ResourceManager& resmgr;
    float volume = 0.f;
    std::optional<std::string> currentTrackName;
};
