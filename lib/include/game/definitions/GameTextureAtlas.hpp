#pragma once

#include <DGM/classes/TextureAtlas.hpp>

struct [[nodiscard]] GameTextureAtlas final
{
    dgm::TextureAtlas atlas;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> bulletLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> explosionLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> playerLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::AnimationStates> bigheadLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> propsLocation;
    dgm::TextureAtlas::ResourceLocation<dgm::Clip> tilesLocation;
    dgm::AnimationStates propsStates;
};
