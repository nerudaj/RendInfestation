#pragma once

#include "game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/dgm.hpp>
#include <string>
#include <vector>

class [[nodiscard]] AppStateWeaponModification final : public dgm::AppState
{
public:
    AppStateWeaponModification(
        dgm::App& app, DependencyContainer& dic, GameScene& scene);

public:
    void input() override;
    void update() override;
    void draw() override;

    void renderWorkbench();

private:
    void buildLayout();
    void _buildLayout();

    std::array<WeaponModule, 3>& getCurrentLoadout()
    {
        return currentWeaponIdx == 0 ? scene.loadout.weapon1Modules
                                     : scene.loadout.weapon2Modules;
    }

    const std::array<WeaponModule, 3>& getCurrentLoadout() const
    {
        return currentWeaponIdx == 0 ? scene.loadout.weapon1Modules
                                     : scene.loadout.weapon2Modules;
    }

    void onResume();
    void onBack();
    void onCycle();
    void onModSelected(size_t moduleIdx);

    [[nodiscard]] std::vector<std::string> getModuleNames() const;
    [[nodiscard]] std::vector<std::string> getAvailableModuleNames() const;
    [[nodiscard]] std::vector<WeaponModule> getAvailableModules() const;
    [[nodiscard]] static size_t moduleToIndex(WeaponModule module) noexcept;
    [[nodiscard]] static WeaponModule indexToModule(size_t index) noexcept;

private:
    DependencyContainer& dic;
    GameScene& scene;

    dgm::Camera renderCamera;
    dgm::Camera guiCamera;

    const sf::Texture& workbenchTexture;
    const dgm::Clip& workbenchClip;
    sf::Sprite workbenchSprite;

    const sf::Texture& moduleIconTexture;
    const dgm::Clip& moduleIconClip;
    sf::Sprite moduleIconSprite;

    int currentWeaponIdx = 0;
};
