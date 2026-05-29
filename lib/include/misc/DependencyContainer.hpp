#pragma once

#include "audio/Jukebox.hpp"
#include "audio/SoundPlayer.hpp"
#include "filesystem/ResourceLoader.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "gui/Icon.hpp"
#include "input/Input.hpp"
#include "input/TouchController.hpp"
#include "input/VirtualCursor.hpp"
#include "settings/AppSettings.hpp"
#include "strings/Language.hpp"
#include "strings/StringId.hpp"
#include <DGM/dgm.hpp>
#include <filesystem/AppStorage.hpp>
#include <functional>
#include <gui/Gui.hpp>
#include <gui/GuiBuilderFactory.hpp>
#include <gui/Sizers.hpp>
#include <strings/StringProvider.hpp>

struct [[nodiscard]] DependencyContainer final
{
    Gui gui;
    dgm::ResourceManager resmgr;
    const StringProvider<StringId> strings;
    TouchController touchController;
    Input input;
    VirtualCursor virtualCursor;
    Sizer sizer;
    Jukebox jukebox;
    SoundPlayer soundPlayer;
    AppSettings settings;
    std::function<void()> saveSettings;
    GuiBuilderFactory<StringId> guiBuilderFactory;

    DependencyContainer(
        dgm::Window& window,
        const std::filesystem::path& rootDir,
        Language primaryLang,
        const AppSettingsStorageModel& settingsSM,
        const std::filesystem::path& settingsFileName);

    const tgui::Texture& getIcon(Icon icon) const;
};
