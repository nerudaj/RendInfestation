#include "filesystem/ResourceLoader.hpp"
#include "filesystem/TiledLoader.hpp"
#include "misc/CMakeVars.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <expected>
#include <filesystem/AppStorage.hpp>
#include <filesystem/DgmLoader.hpp>
#include <filesystem/SfmlLoader.hpp>
#include <filesystem/TguiLoader.hpp>
#include <gui/TguiHelper.hpp>
#include <misc/Compatibility.hpp>

static std::expected<tiled::FiniteMapModel, dgm::Error>
loadTiledMap(const std::filesystem::path& path)
{
    try
    {
        return TiledLoader::loadLevel(path);
    }
    catch (const std::exception& ex)
    {
        return std::unexpected { dgm::Error(ex.what()) };
    }
}

class Loader
{
public:
    Loader(dgm::ResourceManager& resmgr, const std::filesystem::path& assetDir)
        : resmgr(resmgr), assetDir(assetDir)
    {
    }

    Loader(Loader&&) = delete;
    Loader(const Loader&) = delete;

public:
    template<class T>
    void loadOrThrow(
        const std::string& folder,
        const std::vector<std::string>& extensions,
        auto&& loadCallback)
    {
        if (auto result = resmgr.loadResourcesFromDirectory<T>(
                assetDir / folder,
                std::forward<decltype(loadCallback)>(loadCallback),
                extensions);
            !result)
        {
            throw dgm::Exception(uni::format(
                "Could not load resource: {}", result.error().getMessage()));
        }
    }

private:
    dgm::ResourceManager& resmgr;
    std::filesystem::path assetDir;
};

static dgm::ExpectedSuccess loadTguiIcons(
    dgm::ResourceManager& resmgr,
    const std::string& sourceIconName,
    const std::string& targetResourceName)
{
    auto&& iconTexture = resmgr.get<sf::Texture>(sourceIconName);
    auto&& iconClip = resmgr.get<dgm::Clip>(sourceIconName + ".clip");

    tgui::Texture::setDefaultSmooth(false);

    for (auto&& idx : std::views::iota(0u, iconClip.getFrameCount()))

    {
        auto&& texture =
            TguiHelper::convertTexture(iconTexture, iconClip.getFrame(idx));
        assert(!texture.isSmooth());

        auto result = resmgr.insertResource<tgui::Texture>(
            uni::format("{}-{}", targetResourceName, idx), std::move(texture));
        if (!result) return result;
    }

    return std::true_type {};
}

dgm::ResourceManager
ResourceLoader::loadResources(const std::filesystem::path& assetDir)
{
    dgm::ResourceManager resmgr;
    auto&& loader = Loader(resmgr, assetDir);

    loader.loadOrThrow<sf::Font>("fonts", { ".ttf" }, dgm::Utility::loadFont);
    loader.loadOrThrow<tgui::Font>("fonts", { ".ttf" }, TguiLoader::loadFont);
    loader.loadOrThrow<tgui::Theme::Ptr>(
        "ui-themes", { ".txt" }, TguiLoader::loadTheme);
    loader.loadOrThrow<tgui::Texture>(
        "tgui-graphics", { ".png" }, TguiLoader::loadTexture);
    loader.loadOrThrow<sf::Texture>(
        "graphics", { ".png" }, SfmlLoader::loadTexture);
    loader.loadOrThrow<dgm::AnimationStates>(
        "graphics", { ".anim" }, DgmLoader::loadAnimationStates);
    loader.loadOrThrow<dgm::Clip>("graphics", { ".clip" }, DgmLoader::loadClip);
    loader.loadOrThrow<sf::SoundBuffer>(
        "sounds", { ".wav" }, SfmlLoader::loadSound);
    loader.loadOrThrow<sf::Music>(
        "music", { ".ogg", ".wav" }, SfmlLoader::loadSong);
    loader.loadOrThrow<tiled::FiniteMapModel>(
        "levels", { ".json" }, loadTiledMap);

    if (auto result =
            loadTguiIcons(resmgr, "infestation_modules.png", "ModuleIcon");
        !result)
    {
        throw std::runtime_error(uni::format(
            "Could not load TGUI icons: {}", result.error().getMessage()));
    }

    if (auto result = loadTguiIcons(resmgr, "pixel-ui-icons.png", "Icon");
        !result)
    {
        throw std::runtime_error(uni::format(
            "Could not load TGUI icons: {}", result.error().getMessage()));
    }

#ifndef ANDROID
    // Shaders are not supported on Android with SFML
    if (auto result = resmgr.loadResource<sf::Shader>(
            assetDir / "shaders" / "wave", SfmlLoader::loadShader);
        !result)
    {
        throw std::runtime_error(uni::format(
            "Could not load shader: {}", result.error().getMessage()));
    }
#endif

    return resmgr;
}

AppSettingsStorageModel
ResourceLoader::loadSettings(const std::filesystem::path& file)
{
    auto settingsJson = AppStorage::loadFile(CMakeVars::TITLE, file);

    if (settingsJson)
    {
        try
        {
            AppSettingsStorageModel settings =
                nlohmann::json::parse(settingsJson.value());
            return settings;
        }
        catch (const std::exception& ex)
        {
            sf::err() << ex.what() << std::endl;
        }
    }
    else
    {
        sf::err() << settingsJson.error().getMessage() << std::endl;
    }

    return AppSettingsStorageModel {};
}
