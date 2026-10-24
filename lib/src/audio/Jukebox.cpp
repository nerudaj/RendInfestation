#include "audio/Jukebox.hpp"

const sf::Time INFINITE_TIME = sf::seconds(
    24 * 60 * 60); ///< numeric_limits::infinity yields negative time

Jukebox::Jukebox(dgm::ResourceManager& resmgr) : resmgr(resmgr)
{
    workerThread = std::thread(&Jukebox::worker, this);
}

Jukebox::~Jukebox()
{
    stopRequested = true;
    cv.notify_all();
    workerThread.join();
}

void Jukebox::setVolume(float newVolume)
{
    assert(0.f <= newVolume && newVolume <= 100.f);
    volume = newVolume;

    auto&& guard = std::unique_lock(mutex);
    if (currentTrackName) getCurrentTrack().setVolume(volume);
}

sf::Music& Jukebox::getCurrentTrack()
{
    assert(currentTrackName);
    return resmgr.getMutable<sf::Music>(currentTrackName.value());
}

void Jukebox::writeCommand(JukeboxCommand command)
{
    auto&& lock = std::unique_lock(mutex);
    commands.push(command);
    cv.notify_one();
}

sf::Time Jukebox::executeCommand(
    JukeboxCommand command, const std::unique_lock<std::mutex>&)
{
    switch (command.kind)
    {
        using enum JukeboxCommandKind;

    case Stop:
        if (currentTrackName) getCurrentTrack().stop();
        currentTrackName.reset();
        break;

    case Pause:
        if (currentTrackName) getCurrentTrack().pause();
        break;

    case Play:
        return playRandomTrack();

    case Resume: {
        assert(currentTrackName);
        auto& track = getCurrentTrack();
        track.play();
        return track.getDuration() - track.getPlayingOffset();
    }

    case ChangeList:
        if (currentTrackName) getCurrentTrack().stop();
        playlist = std::move(command.data);
        return playRandomTrack();
    }

    return INFINITE_TIME;
}

void Jukebox::worker()
{
    auto duration = INFINITE_TIME;

    while (!stopRequested)
    {
        assert(duration > sf::Time::Zero);
        auto&& lock = std::unique_lock(mutex);
        cv.wait_for(
            lock,
            duration.toDuration(),
            [&] { return !commands.empty() || stopRequested; });

        if (stopRequested)
        {
            return;
        }
        else if (!commands.empty())
        {
            const auto command = commands.front();
            commands.pop();
            duration = executeCommand(command, lock);
        }
        else // song finished
        {
            duration = playRandomTrack();
        }
    }
}

sf::Time Jukebox::playRandomTrack()
{
    currentTrackName = playlist[rand() % playlist.size()];

    auto& track = getCurrentTrack();
    track.setVolume(volume);
    track.setLooping(false);
    track.play();
    return track.getDuration();
}
