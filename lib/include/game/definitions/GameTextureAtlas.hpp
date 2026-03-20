#pragma once

#include "game/enums/SkinType.hpp"
#include <DGM/classes/TextureAtlas.hpp>

struct [[nodiscard]] GameTextureAtlas final
{
    dgm::TextureAtlas atlas;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>
        smallBulletLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> bulletLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> landmineLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> spikesLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> hyperbeamLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> explosionLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> playerLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> bigheadLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>
        scuttlebugLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> propsLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>
        doorHorizontalLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> tilesLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> crosshairsLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> lightsLocation;

    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>
    getSkinLocation(SkinType skin) const
    {
        switch (skin)
        {
            using enum SkinType;
        case PlayerDefault:
            return playerLocation;
        case Bighead:
            return bigheadLocation;
        case Scuttlebug:
            return scuttlebugLocation;
        case SmallBullet:
            return smallBulletLocation;
        case BigBullet:
            return bulletLocation;
        case Landmine:
            return landmineLocation;
        case Hyperbeam:
            return hyperbeamLocation;
        case Spikes:
            return spikesLocation;
        case Prop:
            return propsLocation;
        case Explosion:
            return explosionLocation;
        case DoorHorizontal:
            return doorHorizontalLocation;
        }

        assert(false);
    }

    dgm::AnimationStates getSkinAnimationStates(SkinType skin) const
    {
        return atlas.getAnimationStates(getSkinLocation(skin));
    }
};
