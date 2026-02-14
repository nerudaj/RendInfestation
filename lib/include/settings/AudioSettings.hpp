#pragma once

#include <nlohmann/json.hpp>
#include <types/Observable.hpp>

struct [[nodiscard]] AudioSettingsStorageModel final
{
    float soundVolume = 50.f;
    float musicVolume = 50.f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    AudioSettingsStorageModel, soundVolume, musicVolume);

struct [[nodiscard]] AudioSettings final
{
    Observable<float> soundVolume;
    Observable<float> musicVolume;
};
