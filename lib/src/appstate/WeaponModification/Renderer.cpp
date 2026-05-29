#include "appstate/WeaponModification/Renderer.hpp"
#include "misc/Compatibility.hpp"

enum class [[nodiscard]] WorkbenchSpriteId
{
    Ui,
    BigMag,
    BigNozzle,
    LongBarrel,
    BaseGun,
    Table,
    Ammo,
    Mine,
    Spikes
};

Renderer::Renderer(GameScene& scene, const DependencyContainer& dic)
    : scene(scene)
    , workbenchTexture(dic.resmgr.get<sf::Texture>("workbench.png"))
    , workbenchClip(dic.resmgr.get<dgm::Clip>("workbench.png.clip"))
    , workbenchSprite(workbenchTexture)
    , moduleIconTexture(dic.resmgr.get<sf::Texture>("infestation_modules.png"))
    , moduleIconClip(dic.resmgr.get<dgm::Clip>("infestation_modules.png.clip"))
    , moduleIconSprite(moduleIconTexture)
{
}

static float easeInOut(float x)
{
    return x < 0.5f ? 4 * std::pow(x, 3.f) : 1 - std::pow(-2 * x + 2, 3.f) / 2;
}

void Renderer::renderWorkbench(
    dgm::Window& window, const float animationFactor, size_t currentWeaponIdx)
{
    auto getCurrentLoadout = [&]()
    {
        return currentWeaponIdx == 0 ? scene.loadout.weapon1Modules
                                     : scene.loadout.weapon2Modules;
    };

    workbenchSprite.setPosition({ 0.f, 0.f });
    workbenchSprite.setTextureRect(
        workbenchClip.getFrame(WorkbenchSpriteId::Table));
    window.draw(workbenchSprite);

    if (animationFactor > 0.f)
    {
        // const float factor = 1.f - animationTimer / ANIMATION_DURATION;
        const float offset =
            easeInOut(animationFactor) * INTERNAL_GAME_RESOLUTION.x;
        renderWeapon(
            window,
            offset,
            currentWeaponIdx == 0 ? scene.loadout.weapon2Modules
                                  : scene.loadout.weapon1Modules);
        renderWeapon(
            window, offset - INTERNAL_GAME_RESOLUTION.x, getCurrentLoadout());
    }
    else
    {
        renderWeapon(window, 0.f, getCurrentLoadout());
    }

    // Don't render UI buttons while transitioning
    if (animationFactor > 0.f) return;

    workbenchSprite.setTextureRect(
        workbenchClip.getFrame(WorkbenchSpriteId::Ui));
    window.draw(workbenchSprite);

    for (auto&& [idx, module] : uni::views::enumerate(getCurrentLoadout()))
    {
        if (module == WeaponModule::None) continue;
        moduleIconSprite.setTextureRect(moduleIconClip.getFrame(module));
        moduleIconSprite.setPosition({ 36.f + idx * 23.f, 164.f });
        window.draw(moduleIconSprite);
    }
}

void Renderer::renderWeapon(
    dgm::Window& window,
    float xOffset,
    const std::array<WeaponModule, 3>& loadout)
{
    workbenchSprite.setPosition({ xOffset, 0.f });

    workbenchSprite.setTextureRect(
        workbenchClip.getFrame(WorkbenchSpriteId::BaseGun));
    window.draw(workbenchSprite);

    if (uni::ranges::contains(loadout, WeaponModule::BigBullet))
    {
        workbenchSprite.setTextureRect(
            workbenchClip.getFrame(WorkbenchSpriteId::BigMag));
        window.draw(workbenchSprite);
    }

    if (uni::ranges::contains(loadout, WeaponModule::CadenceBarrel))
    {
        workbenchSprite.setTextureRect(
            workbenchClip.getFrame(WorkbenchSpriteId::LongBarrel));
        window.draw(workbenchSprite);
    }

    if (uni::ranges::contains(loadout, WeaponModule::SpreadBarrel_x2)
        || uni::ranges::contains(loadout, WeaponModule::SpreadBarrel_x4))
    {
        workbenchSprite.setTextureRect(
            workbenchClip.getFrame(WorkbenchSpriteId::BigNozzle));
        window.draw(workbenchSprite);
    }
}
