#pragma once

#include <DGM/classes/TextureAtlas.hpp>

struct [[nodiscard]] GameTextureAtlas final
{
    dgm::TextureAtlas atlas;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates>
        mrguyAnimationsLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> tilesLocation;
};
