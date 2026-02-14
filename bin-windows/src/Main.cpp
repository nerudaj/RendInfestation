#include <DGM/dgm.hpp>
#include <appstate/AppStateMainMenu.hpp>
#include <filesystem/AppStorage.hpp>
#include <misc/CMakeVars.hpp>
#include <misc/DependencyContainer.hpp>

const auto SETTINGS_FILE_NAME = std::filesystem::path("settings.json");

int main(int, char*[])
{
    try
    {
        auto&& settings = ResourceLoader::loadSettings(SETTINGS_FILE_NAME);

        auto&& window = dgm::Window(dgm::WindowSettings {
            .resolution = settings.video.resolution,
            .title = CMakeVars::TITLE,
            .useFullscreen = settings.video.fullscreen,
        });
        auto&& app = dgm::App(window);

        try
        {
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
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            return 1;
        }
    }
    catch (const std::exception& ex)
    {
        // streams are too botched at this point for logging
        throw;
    }

    return 0;
}
