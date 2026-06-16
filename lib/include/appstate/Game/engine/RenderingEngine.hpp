#pragma once

#include "appstate/Game/definitions/Face.hpp"
#include "appstate/Game/definitions/GameScene.hpp"
#include "appstate/Game/definitions/GameTextureAtlas.hpp"
#include "input/TouchController.hpp"
#include "misc/FpsCounter.hpp"
#include "misc/ShadeableRenderingPipeline2D.hpp"
#include "settings/AppSettings.hpp"
#include "strings/StringId.hpp"
#include <DGM/dgm.hpp>
#include <strings/StringProvider.hpp>

class [[nodiscard]] RenderingEngine final
{
public:
    RenderingEngine(
        dgm::ResourceManager& resmgr,
        GameScene& scene,
        const GameTextureAtlas& atlas,
        const AppSettings& settings,
        const TouchController& touchController,
        const StringProvider<StringId>& strings) noexcept;

    RenderingEngine(RenderingEngine&&) = delete;
    RenderingEngine(const RenderingEngine&) = delete;

public:
    void update(const dgm::Time& time);

    void updateCameraPosition(const dgm::Time& time);

    void draw(dgm::Window& window);

private:
    static dgm::Camera createFullscreenCamera(
        const sf::Vector2f& currentResolution,
        const sf::Vector2f& desiredResolution);

    void renderWorld(dgm::Window& window);

    void addLevelFacesToPipeline();

    void addParticlesToPipeline();

    void renderColliders(dgm::Window& window);

    void renderLights(dgm::Window& window);

    void renderHud(dgm::Window& window);

    void renderHudBackgroundAndHealth(
        const sf::Vector2f& hudOrigin, dgm::Window& window);

    void renderHudReloadTimeAndModules(
        const sf::Vector2f& hudOrigin, dgm::Window& window);

    void renderHudStrings(dgm::Window& window);

    void renderTouchControls(dgm::Window& window);

    sf::Angle getEntityRotation(entt::entity entity, EntityKind kind) const;

private:
    sf::FloatRect getFrame(
        SkinType skin,
        const std::string& stateName,
        const size_t frameIdx) const
    {
        return sf::FloatRect {
            atlas.getSkinAnimationStates(skin).at(stateName).getFrame(frameIdx)
        };
    }

    std::vector<Face> getActorFaces() const;

    bool isObjectVisible(const dgm::Circle& o) const
    {
        return dgm::Collision::basic(viewportCollider, o);
    }

    bool isObjectVisible(const dgm::Rect& o) const
    {
        return dgm::Collision::basic(viewportCollider, o);
    }

private:
    GameScene& scene;
    const GameTextureAtlas& atlas;
    const AppSettings& settings;
    const TouchController& touchController;
    const StringProvider<StringId>& strings;
    const dgm::ResourceManager& resmgr;
    dgm::Camera worldCamera;
    dgm::Camera hudCamera;
    dgm::Camera touchInputCamera;
    dgm::Rect viewportCollider;

    FpsCounter fpsCounter;
    sf::Text text;
    sf::Sprite hudSprite;

    ShadeableRenderingPipeline2D pipeline;
    ShadeableRenderingPipeline2D lightPipeline;
    const dgm::Clip& tilesClip;
    const dgm::Clip& hudClip;
    const dgm::Clip& iconsClip;
    const dgm::Clip& modulesClip;
    float timeElapsed = 0.f;
    mutable size_t entityCount = 0;
    sf::Vector2f cameraPosition = { 0.f, 0.f };
};
