#pragma once

#include "audio/Playlist.hpp"
#include <DGM/classes/ResourceManager.hpp>
#include <SFML/Audio/Music.hpp>
#include <atomic>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

class [[nodiscard]] Jukebox final
{
public:
    explicit Jukebox(dgm::ResourceManager& resmgr);

    Jukebox(const Jukebox&) = delete;
    Jukebox(Jukebox&&) = delete;
    ~Jukebox();

public:
    void setVolume(float newVolume);

    inline void play()
    {
        writeCommand({ JukeboxCommandKind::Play });
    }

    inline void resume()
    {
        writeCommand({ JukeboxCommandKind::Resume });
    }

    inline void pause()
    {
        writeCommand({ JukeboxCommandKind::Pause });
    }

    inline void stop()
    {
        writeCommand({ JukeboxCommandKind::Stop });
    }

    inline void changePlaylist(const std::vector<std::string>& list)
    {
        writeCommand({ JukeboxCommandKind::ChangeList, list });
    }

private:
    enum class [[nodiscard]] JukeboxCommandKind
    {
        Stop,
        Pause,
        Play,
        Resume,
        ChangeList
    };

    struct [[nodiscard]] JukeboxCommand final
    {
        JukeboxCommandKind kind;
        std::vector<std::string> data;
    };

private:
    void writeCommand(JukeboxCommand command);

    sf::Time
    executeCommand(JukeboxCommand command, const std::unique_lock<std::mutex>&);

    void worker();

    sf::Time playRandomTrack();

private:
    sf::Music& getCurrentTrack();

private:
    dgm::ResourceManager& resmgr;

    float volume = 0.f;
    std::optional<std::string> currentTrackName;
    std::vector<std::string> playlist;

    std::atomic_bool stopRequested =
        false; ///< stop token is not supported on all compilers
    std::mutex mutex;
    std::thread workerThread;
    std::condition_variable cv;
    std::queue<JukeboxCommand>
        commands; ///< Used to signal what worker thread shall do
};
