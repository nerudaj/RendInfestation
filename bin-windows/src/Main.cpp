#include <DGM/dgm.hpp>
#include <appstate/AppStateMainMenu.hpp>
#include <filesystem/AppStorage.hpp>
#include <misc/CMakeVars.hpp>
#include <misc/DependencyContainer.hpp>

const auto SETTINGS_FILE_NAME = std::filesystem::path("settings.json");

int main(int, char*[])
{
#ifndef _DEBUG
    try
    {
#endif
        auto&& settings = ResourceLoader::loadSettings(SETTINGS_FILE_NAME);

        auto&& window = dgm::Window(dgm::WindowSettings {
            .resolution = settings.video.resolution,
            .title = CMakeVars::TITLE,
            .useFullscreen = settings.video.fullscreen,
        });
        auto&& app = dgm::App(window);

#ifndef _DEBUG
        try
        {
#endif
            auto&& dependencies = DependencyContainer(
                window,
                "../assets",
                Language::English,
                settings,
                SETTINGS_FILE_NAME);

            window.getSfmlWindowContext().setMouseCursorVisible(false);

            app.pushState<AppStateMainMenu>(dependencies);
            app.run();

            dependencies.saveSettings();
#ifndef _DEBUG
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            throw;
        }
    }
    catch (const std::exception&)
    {
        // streams are too botched at this point for logging
        throw;
    }
#endif

    return 0;
}
