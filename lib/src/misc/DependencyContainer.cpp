#include "misc/DependencyContainer.hpp"
#include "localization/EnLocalization.hpp"
#include "misc/CMakeVars.hpp"

DependencyContainer::DependencyContainer(
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
    , strings(
          primaryLang,
          std::map<Language, Localization<StringId>> {
              { Language::English, EN_LOCALIZATION } })
    , touchController(settingsSM.video.resolution, sizer)
    , input(settingsSM.bindings, touchController, window.getSfmlWindowContext())
    , virtualCursor(
          window.getSfmlWindowContext(),
          input,
          resmgr.get<sf::Texture>("cursor.png"))
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
                      [&](float newVolume) { jukebox.setVolume(newVolume); }),
              },
          .video =
              VideoSettings {
                  .resolution = UnsafeObservable<sf::Vector2u>(
                      settingsSM.video.resolution,
                      [&](const sf::Vector2u& newResolution)
                      {
                          touchController.recomputeLayoutAfterWindowResize(
                              newResolution);
                      }),
                  .fullscreen = settingsSM.video.fullscreen,
                  .uiScale = Observable<float>(
                      settingsSM.video.uiScale,
                      [&](float newScale) { sizer.setScale(newScale); }),
              },
          .input = settingsSM.input,
          .bindings = settingsSM.bindings,
      })
    , saveSettings(
          [settingsFileName, this]
          {
              AppStorage::saveFile(
                  CMakeVars::TITLE,
                  settingsFileName,
                  AppSettingsStorageModel(settings));
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

    gui.setFont(resmgr.get<tgui::Font>(FONT_NAME));
    // NOTE: You can create your own theme file and use it here
    gui.setTheme(resmgr.get<tgui::Theme::Ptr>("Pico8.txt"));
}

const tgui::Texture& DependencyContainer::getIcon(Icon icon) const
{
    return resmgr.get<tgui::Texture>(
        uni::format("Icon-{}", std::to_underlying(icon)));
}
