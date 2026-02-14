#include "audio/Jukebox.hpp"

void Jukebox::setVolume(float newVolume)
{
    assert(0.f <= newVolume && newVolume <= 100.f);
    volume = newVolume;

    if (currentTrackName) getCurrentTrack().setVolume(volume);
}

sf::Music& Jukebox::getCurrentTrack()
{
    assert(currentTrackName);
    return resmgr.getMutable<sf::Music>(currentTrackName.value());
}

void Jukebox::play(const std::string& trackName, bool loop)
{
    if (trackName.empty())
    {
        currentTrackName.reset();
        return;
    }
    else if (currentTrackName.value_or("") == trackName)
        return;

    currentTrackName = trackName;
    auto& track = getCurrentTrack();
    track.setVolume(volume);
    track.setLooping(loop);
    track.play();
}

void Jukebox::stop()
{
    getCurrentTrack().stop();
}

void Jukebox::resume()
{
    if (!currentTrackName) return;
    getCurrentTrack().play();
}
