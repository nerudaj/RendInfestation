#pragma once

#include "audio/Jukebox.hpp"
#include "audio/SoundPlayer.hpp"
#include "filesystem/AppStorage.hpp"
#include "filesystem/ResourceLoader.hpp"
#include "gui/Gui.hpp"
#include "gui/GuiBuilderFactory.hpp"
#include "gui/Sizers.hpp"
#include "input/Input.hpp"
#include "input/TouchController.hpp"
#include "input/VirtualCursor.hpp"
#include "settings/AppSettings.hpp"
#include "strings/StringProvider.hpp"
#include <DGM/dgm.hpp>
#include <functional>

struct [[nodiscard]] DependencyContainer final
{
    Gui gui;
    dgm::ResourceManager resmgr;
    const StringProvider strings;
    TouchController touchController;
    Input input;
    VirtualCursor virtualCursor;
    Sizer sizer;
    Jukebox jukebox;
    SoundPlayer soundPlayer;
    AppSettings settings;
    std::function<void()> saveSettings;
    GuiBuilderFactory guiBuilderFactory;

    DependencyContainer(
        dgm::Window& window,
        const std::filesystem::path& rootDir,
        Language primaryLang,
        const AppSettingsStorageModel& settingsSM,
        const std::filesystem::path& settingsFileName)
        // Gui needs to be instantiated before Resource manager
        // since we need to have gui backend defined before
        // other tgui objects (like fonts) can be created.
        : gui(window)
        , resmgr(ResourceLoader::loadResources(rootDir))
        , strings(primaryLang)
        , touchController(settingsSM.video.resolution)
        , input(settingsSM.bindings, touchController)
        , virtualCursor(
              window.getSfmlWindowContext(),
              input,
              resmgr.get<sf::Texture>("cursor.png"))
        , sizer(settings.video)
        , jukebox(resmgr)
        , soundPlayer(resmgr)
        , settings(AppSettings {
              .audio =
                  AudioSettings {
                      .soundVolume = Observable<float>(
                          settingsSM.audio.soundVolume,
                          [&](float newVolume)
                          { soundPlayer.setVolume(newVolume); }),
                      .musicVolume = Observable<float>(
                          settingsSM.audio.musicVolume,
                          [&](float newVolume)
                          { jukebox.setVolume(newVolume); }),
                  },
              .video = settingsSM.video,
              .input = settingsSM.input,
              .bindings = settingsSM.bindings,
          })
        , saveSettings(
              [settingsFileName, this]
              {
                  AppStorage::saveFile(
                      settingsFileName, AppSettingsStorageModel(settings));
              })
        , guiBuilderFactory(sizer, strings, soundPlayer)
    {
        // These asserts will fail when you're adding new bindings
        // just delete the settings.json and all will be fine again
        assert(
            BindingsSettings {}.ingameBindings.size()
            == settingsSM.bindings.ingameBindings.size());
        assert(
            BindingsSettings {}.menuBindings.size()
            == settingsSM.bindings.menuBindings.size());

        gui.setFont(resmgr.get<tgui::Font>("ChunkFive-Regular.ttf"));
        // NOTE: You can create your own theme file and use it here
        gui.setTheme(resmgr.get<tgui::Theme::Ptr>("Pico8.txt"));
    }
};
