#pragma once

#include "audio/SoundChannel.hpp"
#include "audio/SoundId.hpp"
#include <DGM/classes/ResourceManager.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <vector>

class [[nodiscard]] SoundPlayer final
{
public:
    explicit SoundPlayer(const dgm::ResourceManager& resmgr);
    SoundPlayer(const SoundPlayer&) = delete;
    SoundPlayer(SoundPlayer&&) = default;

public:
    /**
     *  Play a sound that is not attenuated by distance.
     *  This is used for sounds that are played from the
     *  player's point of view, such as weapon sounds or UI sounds.
     *
     * \returns The duration of the sound that started playing.
     */
    sf::Time playPovSound(SoundId::IdType soundId)
    {
        return playSound(soundId, SoundChannel::POV, 0.f);
    }

    /**
     * Play a sound that is attenuated by distance.
     * This is used for sounds that are played from the environment, such as
     * enemy sounds or ambient sounds. The distance parameter is used to
     * calculate the volume of the sound based on its distance from the player.
     *
     * \returns The duration of the sound that started playing.
     */
    sf::Time playAttenuatedSound(
        SoundChannel channel, SoundId::IdType soundId, const float distance)
    {
        return playSound(soundId, channel, distance);
    }

    void stopChannel(SoundChannel channel);

    void stopAllChannels();

    void setVolume(const float volume);

private:
    sf::Time playSound(
        SoundId::IdType soundId, SoundChannel channel, const float distance);

private:
    struct ChannelGroup
    {
        size_t slotIdx = 0;
        std::vector<sf::Sound> slots;
    };

private:
    const dgm::ResourceManager& resmgr;
    sf::SoundBuffer emptySoundBuffer = {};
    std::map<SoundChannel, ChannelGroup> channels = {
        { SoundChannel::POV,
          ChannelGroup {
              .slots = std::vector<sf::Sound>(4, sf::Sound(emptySoundBuffer)),
          } },
        { SoundChannel::Enemy,
          ChannelGroup {
              .slots = std::vector<sf::Sound>(8, sf::Sound(emptySoundBuffer)),
          } },
        { SoundChannel::Ambient,
          ChannelGroup {
              .slots = std::vector<sf::Sound>(2, sf::Sound(emptySoundBuffer)),
          } }
    };
};
