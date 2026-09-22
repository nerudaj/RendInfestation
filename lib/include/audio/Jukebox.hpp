#pragma once

#include <DGM/classes/ResourceManager.hpp>
#include <SFML/Audio/Music.hpp>
#include <atomic>
#include <mutex>
#include <optional>
#include <stop_token>
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

    ~Jukebox()
    {
        cv.notify_all();
    }

public:
    void setVolume(float newVolume);

    void play(JukeboxMode newMode);

    void stop();

    void resume();

private:
    void worker(const std::stop_token& stopToken);

    sf::Time playRandomTrack();

private:
    sf::Music& getCurrentTrack();

private:
    dgm::ResourceManager& resmgr;
    float volume = 0.f;
    std::optional<std::string> currentTrackName;
    std::atomic<JukeboxMode> mode;
    std::atomic<JukeboxStatus> status;
    std::mutex mutex;
    std::jthread workerThread;
    std::condition_variable cv;
};
