#pragma once

#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "audio/SoundPlayer.hpp"

class [[nodiscard]] AudioUtility final
{
public:
    AudioUtility(const GameScene& scene, SoundPlayer& player) noexcept
        : scene(scene), player(player)
    {
    }

public:
    inline void playPovSound(
        SoundId::IdType soundId,
        const SoundPostProcess postProcess = SoundPostProcess::None)
    {
        std::ignore = player.playPovSound(soundId, postProcess);
    }

    void playAttenuated(
        SoundId::IdType soundId,
        const Collider& collider,
        SoundChannel channel = SoundChannel::Enemy);

private:
    const GameScene& scene;
    SoundPlayer& player;
};
