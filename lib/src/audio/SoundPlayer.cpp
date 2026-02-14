#include "audio/SoundPlayer.hpp"

SoundPlayer::SoundPlayer(const dgm::ResourceManager& resmgr) : resmgr(resmgr) {}

void SoundPlayer::stopChannel(SoundChannel channel)
{
    assert(channels.contains(channel));
    auto& group = channels[channel];
    for (auto& slot : group.slots)
    {
        slot.stop();
    }
}

void SoundPlayer::stopAllChannels()
{
    for (auto& [_, group] : channels)
    {
        for (auto& slot : group.slots)
        {
            slot.stop();
        }
    }
}

void SoundPlayer::setVolume(const float volume)
{
    for (auto& [_, group] : channels)
    {
        for (auto& slot : group.slots)
        {
            slot.setVolume(volume);
        }
    }
}

sf::Time SoundPlayer::playSound(
    SoundId::IdType soundId, SoundChannel channel, const float distance)
{
    assert(channels.contains(channel));

    auto& group = channels[channel];
    auto& slot = group.slots[group.slotIdx];
    auto& buffer = resmgr.get<sf::SoundBuffer>(soundId);
    slot.setBuffer(buffer);

    if (channel != SoundChannel::POV)
    {
        slot.setPosition(sf::Vector3f { distance, 0.f, 0.f });
    }

    slot.play();

    group.slotIdx = (group.slotIdx + 1) % group.slots.size();

    return buffer.getDuration();
}
