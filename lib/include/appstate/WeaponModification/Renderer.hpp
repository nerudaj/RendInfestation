#pragma once

#include "appstate/WeaponModification/AnimationTimer.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/classes/Clip.hpp>
#include <DGM/classes/Window.hpp>
#include <SFML/Graphics.hpp>

class [[nodiscard]] Renderer final
{
public:
    Renderer(GameScene& scene, const DependencyContainer& dic);

public:
    void renderWorkbench(
        dgm::Window& window,
        const AnimationTimer& timer,
        size_t currentWeaponIdx);

private:
    void renderWeapon(
        dgm::Window& window,
        float xOffset,
        const std::array<WeaponModule, 3>& loadout);

private:
    GameScene& scene;

    const sf::Texture& workbenchTexture;
    const dgm::Clip& workbenchClip;
    sf::Sprite workbenchSprite;

    const sf::Texture& moduleIconTexture;
    const dgm::Clip& moduleIconClip;
    sf::Sprite moduleIconSprite;
};
