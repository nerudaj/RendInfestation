#pragma once

#include <DGM/classes/ResourceManager.hpp>
#include <SFML/Audio/Music.hpp>
#include <atomic>
#include <mutex>
#include <optional>
#include <thread>

enum class [[nodiscard]] JukeboxMode
{
    Menu,
    Game,
};

enum class [[nodiscard]] JukeboxStatus
{
    Off,
    Resuming,
    Running,
    ChangingMode,
};

class [[nodiscard]] Jukebox final
{
public:
    explicit Jukebox(dgm::ResourceManager& resmgr);

    Jukebox(const Jukebox&) = delete;
    Jukebox(Jukebox&&) = delete;
    ~Jukebox();

public:
    void setVolume(float newVolume);

    void play(JukeboxMode newMode);

    void stop();

    void resume();

private:
    void worker();

    sf::Time playRandomTrack();

private:
    sf::Music& getCurrentTrack();

private:
    dgm::ResourceManager& resmgr;
    float volume = 0.f;
    std::optional<std::string> currentTrackName;
    std::atomic<JukeboxMode> mode;
    std::atomic<JukeboxStatus> status;
    std::atomic_bool stopRequested = false;
    std::mutex mutex;
    std::thread workerThread;
    std::condition_variable cv;
};
