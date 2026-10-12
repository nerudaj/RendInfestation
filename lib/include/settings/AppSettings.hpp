#pragma once

#include "settings/AudioSettings.hpp"
#include "settings/BindingsSettings.hpp"
#include "settings/InputSettings.hpp"
#include "settings/Saves.hpp"
#include "settings/VideoSettings.hpp"
#include <nlohmann/json.hpp>

struct [[nodiscard]] AppSettings final
{
    AudioSettings audio;
    VideoSettings video;
    InputSettings input;
    BindingsSettings bindings;
    Saves saves;
};

struct [[nodiscard]] AppSettingsStorageModel final
{
    AudioSettingsStorageModel audio;
    VideoSettings video;
    InputSettings input;
    BindingsSettings bindings;
    Saves saves;

    AppSettingsStorageModel() = default;

    explicit AppSettingsStorageModel(const AppSettings& settings) noexcept
        : audio({ .soundVolume = settings.audio.soundVolume,
                  .musicVolume = settings.audio.musicVolume })
        , video(settings.video)
        , input(settings.input)
        , bindings(settings.bindings)
        , saves(saves)
    {
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    AppSettingsStorageModel, audio, video, input, bindings, saves);
