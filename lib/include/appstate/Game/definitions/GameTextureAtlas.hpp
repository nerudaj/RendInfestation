#pragma once

#include "appstate/Game/enums/SkinType.hpp"
#include <DGM/classes/TextureAtlas.hpp>
#include <cassert>
#include <map>

struct [[nodiscard]] GameTextureAtlas final
{
    dgm::TextureAtlas atlas;

    std::
        map<SkinType, dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>>
            skinLocations;

    dgm::TextureAtlas::ResourceLocation<dgm::Clip> tilesLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> crosshairsLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> lightsLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> hudLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> iconsLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> modulesLocation;

    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>
    getSkinLocation(SkinType skin) const
    {
        assert(skinLocations.contains(skin));
        return skinLocations.at(skin);
    }

    dgm::AnimationStates getSkinAnimationStates(SkinType skin) const
    {
        return atlas.getAnimationStates(getSkinLocation(skin));
    }
};
