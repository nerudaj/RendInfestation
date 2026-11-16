#include "appstate/Game/utility/AudioUtility.hpp"

void AudioUtility::playAttenuated(
    SoundId::IdType soundId, const Collider& collider, SoundChannel channel)
{
    player.playAttenuatedSound(
        channel,
        soundId,
        (collider.getPosition()
         - scene.actors.get<Collider>(scene.playerEntity).getPosition())
            .length());
}
