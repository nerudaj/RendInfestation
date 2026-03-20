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
        dgm::App& app, DependencyContainer& dic, GameScene& scene) noexcept
        : dgm::AppState(app), dic(dic), scene(scene)
    {
        buildLayout();
    }

public:
    void input() override;
    void update() override;
    void draw() override;

private:
    void buildLayout();
    void onResume();

    [[nodiscard]] std::vector<std::string> getModuleNames() const;
    [[nodiscard]] std::vector<std::string> getAvailableModuleNames() const;
    [[nodiscard]] std::vector<WeaponModule> getAvailableModules() const;
    [[nodiscard]] static size_t moduleToIndex(WeaponModule module) noexcept;
    [[nodiscard]] static WeaponModule indexToModule(size_t index) noexcept;

private:
    DependencyContainer& dic;
    GameScene& scene;
};
