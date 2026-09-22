#include "audio/Jukebox.hpp"
#include "audio/Playlist.hpp"

Jukebox::Jukebox(dgm::ResourceManager& resmgr) : resmgr(resmgr)
{
    workerThread = std::jthread([&](std::stop_token token) { worker(token); });
}

void Jukebox::setVolume(float newVolume)
{
    assert(0.f <= newVolume && newVolume <= 100.f);
    volume = newVolume;

    auto&& guard = std::lock_guard(mutex);
    if (currentTrackName) getCurrentTrack().setVolume(volume);
}

sf::Music& Jukebox::getCurrentTrack()
{
    assert(currentTrackName);
    return resmgr.getMutable<sf::Music>(currentTrackName.value());
}

void Jukebox::play(JukeboxMode newMode)
{
    // mode MUST be exchanged first (spurious wakeup could trigger on status)
    const auto wasInDifferentMode = mode.exchange(newMode) != newMode;

    // clang-format off
    const auto newStatus = wasInDifferentMode
        ? JukeboxStatus::ChangingMode
        : JukeboxStatus::Running;
    // clang-format on
    if (status.exchange(newStatus) != newStatus) cv.notify_one();
}

void Jukebox::stop()
{
    // If was previously running or resuming
    if (status.exchange(JukeboxStatus::Off) != JukeboxStatus::Off)
        cv.notify_one();
}

void Jukebox::resume()
{
    // If was previously stopped
    if (status.exchange(JukeboxStatus::Resuming) == JukeboxStatus::Off)
        cv.notify_one();
}

void Jukebox::worker(const std::stop_token& stopToken)
{
    auto&& playOrResume = [&]
    {
        if (status == JukeboxStatus::Resuming)
        {
            status.store(JukeboxStatus::Running);
            assert(currentTrackName);
            auto&& track = getCurrentTrack();
            auto&& durationLeft =
                track.getDuration() - track.getPlayingOffset();
            getCurrentTrack().play();
            return durationLeft;
        }

        if (status == JukeboxStatus::ChangingMode) getCurrentTrack().stop();

        status.store(JukeboxStatus::Running);
        return playRandomTrack();
    };

    while (!stopToken.stop_requested())
    {
        // While jukebox is off, wait until it is on
        if (status == JukeboxStatus::Off)
        {
            auto&& lock = std::unique_lock(mutex);
            cv.wait(
                lock,
                [&]()
                {
                    return status != JukeboxStatus::Off
                           || stopToken.stop_requested();
                });
        }
        else
        {
            auto&& duration = playOrResume();
            auto&& lock = std::unique_lock(mutex);
            cv.wait_for(
                lock,
                duration.toDuration(),
                [&]()
                {
                    return status != JukeboxStatus::Running
                           || stopToken.stop_requested();
                });
        }
    }
}

sf::Time Jukebox::playRandomTrack()
{
    currentTrackName =
        mode == JukeboxMode::Menu
            ? PLAYLIST.menuSongs[rand() % PLAYLIST.menuSongs.size()]
            : PLAYLIST.gameSongs[rand() % PLAYLIST.gameSongs.size()];

    auto& track = getCurrentTrack();
    track.setVolume(volume);
    track.setLooping(false);
    track.play();
    return track.getDuration();
}
