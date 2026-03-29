#include "game/engine/RenderingEngine.hpp"
#include "game/enums/Hud.hpp"
#include "gui/Icon.hpp"
#include <cmath>

RenderingEngine::RenderingEngine(
    dgm::ResourceManager& resmgr,
    GameScene& scene,
    const GameTextureAtlas& atlas,
    const AppSettings& settings,
    const TouchController& touchController) noexcept
    : scene(scene)
    , atlas(atlas)
    , settings(settings)
    , touchController(touchController)
    , resmgr(resmgr)
    , worldCamera(createFullscreenCamera(
          sf::Vector2f(settings.video.resolution), INTERNAL_GAME_RESOLUTION))
    , hudCamera(
          sf::FloatRect { { 0.f, 0.f }, { 1.f, 1.f } },
          INTERNAL_GAME_RESOLUTION)
    , touchInputCamera(
          sf::FloatRect { { 0.f, 0.f }, { 1.f, 1.f } },
          sf::Vector2f(settings.video.resolution))
    , text(resmgr.get<sf::Font>("ChunkFive-Regular.ttf"))
    , hudSprite(atlas.atlas.getTexture())
    , pipeline(atlas.atlas.getTexture())
    , lightPipeline(atlas.atlas.getTexture(), sf::BlendAdd)
    , tilesClip(atlas.atlas.getClip(atlas.tilesLocation))
    , hudClip(atlas.atlas.getClip(atlas.hudLocation))
    , iconsClip(atlas.atlas.getClip(atlas.iconsLocation))
    , modulesClip(atlas.atlas.getClip(atlas.modulesLocation))
    , cameraPosition(
          scene.actors.get<Collider>(scene.playerEntity).getPosition())
{
    resmgr.getMutable<sf::Font>("ChunkFive-Regular.ttf").setSmooth(false);
}

void RenderingEngine::update(const dgm::Time& time)
{
    worldCamera.update(time);
    fpsCounter.update(time.getDeltaTime());
    timeElapsed += time.getDeltaTime();

    updateCameraPosition(time);
}

void RenderingEngine::updateCameraPosition(const dgm::Time& time)
{
    const auto offset =
        dgm::Math::toUnit(
            scene.actors.get<LookDirection>(scene.playerEntity).get())
        * (settings.input.cameraFollowsCrosshair ? 50.f : 0.f);
    const auto newCameraPosition =
        scene.actors.get<Collider>(scene.playerEntity).getPosition() + offset;

    const auto directionToNew = newCameraPosition - cameraPosition;

    cameraPosition +=
        dgm::Math::toUnit(directionToNew)
        * std::clamp(directionToNew.length(), 0.f, 512.f * time.getDeltaTime());

    worldCamera.setPosition(cameraPosition);
}

void RenderingEngine::draw(dgm::Window& window)
{
    // Render the game world
    window.setViewFromCamera(worldCamera);
    renderWorld(window);

    // Render everything hud-related
    window.setViewFromCamera(hudCamera);
    renderHud(window);

    if (settings.input.showTouchControls)
    {
        window.setViewFromCamera(touchInputCamera);
        renderTouchControls(window);
    }
}

dgm::Camera RenderingEngine::createFullscreenCamera(
    const sf::Vector2f& currentResolution,
    const sf::Vector2f& desiredResolution)
{
    auto&& viewport = sf::FloatRect {
        { 0.f, 0.f },
        { 1.f, 1.f },
    };

    const auto&& desiredAspectRatio = desiredResolution.x / desiredResolution.y;
    const auto&& currentAspectRatio = currentResolution.x / currentResolution.y;

    // When black bars are up and down then we need to normalize the desired
    // Y resolution to match the current Y resolution proportionally
    // and get the ratio for the viewport.
    if (desiredAspectRatio > currentAspectRatio)
    { // black bars up and down
        const float m = currentResolution.x / desiredResolution.x;
        viewport.size.y = m * desiredResolution.y / currentResolution.y;
    }
    // Same as above, but for the X resolution
    else
    { // black bars left and right
        const float m = currentResolution.y / desiredResolution.y;
        viewport.size.x = m * desiredResolution.x / currentResolution.x;
    }

    // one of the sizes will be 1, so NOP
    // the other one will be <1, so it will be centered
    viewport.position.x = (1.f - viewport.size.x) / 2.f;
    viewport.position.y = (1.f - viewport.size.y) / 2.f;

    return dgm::Camera(viewport, sf::Vector2f(desiredResolution));
}

void RenderingEngine::renderWorld(dgm::Window& window)
{
    pipeline.clear();

    addLevelFacesToPipeline();

    for (auto&& face : getActorFaces())
        pipeline.addFace(face.origin, face.texUvs, face.rotation, face.scale);

    pipeline.addFace(
        scene.actors.get<Collider>(scene.playerEntity).getPosition()
            + scene.actors.get<LookDirection>(scene.playerEntity).get(),
        sf::FloatRect {
            atlas.atlas.getClip(atlas.crosshairsLocation).getFrame(0) });

    pipeline.renderTo(window);

    renderLights(window);

    renderColliders(window);
}

void RenderingEngine::addLevelFacesToPipeline()
{
    for (auto y = 0, idx = 0; y < scene.levelMesh.getDataSize().y; ++y)
    {
        for (auto x = 0; x < scene.levelMesh.getDataSize().x; ++x, ++idx)
        {
            auto pos = sf::Vector2f(scene.levelMesh.getVoxelSize()) / 2.f
                       + sf::Vector2f(x, y).componentWiseMul(
                           sf::Vector2f(scene.levelMesh.getVoxelSize()));
            if (!worldCamera.isObjectVisible(dgm::Circle(pos, 16.f))) continue;

            pipeline.addFace(
                pos,
                sf::FloatRect(
                    tilesClip.getFrame(std::abs(scene.levelMesh[idx]))));

            pipeline.addFace(
                pos,
                sf::FloatRect(
                    tilesClip.getFrame(std::abs(scene.decorationsMesh[idx]))));
        }
    }
}

void RenderingEngine::renderColliders(dgm::Window& window)
{
    return;

    for (auto&& [entity, collider] : scene.actors.view<Collider>().each())
    {
        std::visit(
            overloads {
                [&](const dgm::Circle& c)
                { c.debugRender(window, sf::Color(255, 0, 0, 64)); },
                [&](const dgm::Rect& r)
                { r.debugRender(window, sf::Color(255, 0, 0, 64)); },
            },
            collider.shape);
    }
}

void RenderingEngine::renderLights(dgm::Window& window)
{
    sf::RectangleShape blackRect;
    blackRect.setFillColor(sf::Color { 0, 0, 0, 128 });
    blackRect.setSize(INTERNAL_GAME_RESOLUTION);
    blackRect.setPosition(
        worldCamera.getCurrentView().getCenter()
        - INTERNAL_GAME_RESOLUTION / 2.f);
    window.getSfmlWindowContext().draw(blackRect, sf::BlendAlpha);

    lightPipeline.clear();

    for (auto&& light : scene.lights)
    {
        lightPipeline.addFace(
            light.position,
            sf::FloatRect { atlas.atlas.getClip(atlas.lightsLocation)
                                .getFrame(light.spriteId) },
            sf::degrees(0),
            sf::Vector2f { 1.f, 1.f },
            light.color);
    }

    for (auto&& [entity, collider, light] :
         scene.actors.view<Collider, BoundLightEmitter>().each())
    {
        lightPipeline.addFace(
            collider.getPosition(),
            sf::FloatRect { atlas.atlas.getClip(atlas.lightsLocation)
                                .getFrame(light.lightSpriteId) },
            sf::degrees(0),
            sf::Vector2f { 1.f, 1.f },
            light.color);
    }

    lightPipeline.renderTo(window);
}

void RenderingEngine::renderHud(dgm::Window& window)
{
    const auto hudOrigin =
        sf::Vector2f { INTERNAL_GAME_RESOLUTION.x - hudClip.getFrameSize().x,
                       0.f };

    renderHudBackgroundAndHealth(hudOrigin, window);
    renderHudReloadTimeAndModules(hudOrigin, window);

#if defined(ANDROID) || defined(_DEBUG)
    hudSprite.setTextureRect(iconsClip.getFrame(Icon::Pause2));
    hudSprite.setPosition({ 2.f, 2.f });
    window.draw(hudSprite);
#endif

    renderHudStrings(window);
}

void RenderingEngine::renderHudBackgroundAndHealth(
    const sf::Vector2f& hudOrigin, dgm::Window& window)
{
    hudSprite.setTextureRect(hudClip.getFrame(Hud::Background));
    hudSprite.setPosition(hudOrigin);
    window.draw(hudSprite);

    const int health = scene.actors.get<Health>(scene.playerEntity).get();
    const auto tenth = (std::clamp(health, 0, 100) + 9) / 10;
    hudSprite.setTextureRect(
        hudClip.getFrame(std::to_underlying(Hud::Health1) + tenth - 1));
    window.draw(hudSprite);
}

void RenderingEngine::renderHudReloadTimeAndModules(
    const sf::Vector2f& hudOrigin, dgm::Window& window)
{
    const auto& weaponInventory =
        scene.actors.get<WeaponInventory>(scene.playerEntity);
    const auto activeWeapon =
        weaponInventory.weapons[weaponInventory.activeWeapon];
    const auto cooldownFactor =
        (activeWeapon.cooldown - activeWeapon.timeTillFire)
        / activeWeapon.cooldown;

    // Render reload time
    auto&& reloadShape = sf::RectangleShape({ cooldownFactor * 58.f, 2.f });
    reloadShape.setFillColor(COLOR_PURPLE);
    reloadShape.setPosition(hudOrigin + sf::Vector2f { 5.f, 23.f });
    window.draw(reloadShape);

    // Render used modules:
    const auto& loadout = weaponInventory.activeWeapon
                              ? scene.loadout.weapon2Modules
                              : scene.loadout.weapon1Modules;
    for (auto&& [idx, module] : std::views::enumerate(loadout))
    {
        if (module == WeaponModule::None) continue;
        hudSprite.setTextureRect(
            modulesClip.getFrame(std::to_underlying(module)));
        hudSprite.setPosition(
            hudOrigin + sf::Vector2f { 4.f + idx * 19.f, 4.f });
        window.draw(hudSprite);
    }
}

void RenderingEngine::renderHudStrings(dgm::Window& window)
{
    text.setCharacterSize(10);
    text.setPosition({ 10.f, 10.f });
    text.setString(fpsCounter.getText());
    window.draw(text);

    if (scene.survivalSpawnerContext.wave != -1)
    {
        if (scene.survivalSpawnerContext.state
            == SurvivalModeState::WaitingForNextWave)
        {
            text.setString(uni::format(
                "Next wave in: {}s",
                std::ceil(scene.survivalSpawnerContext.timeTillNextWave
                              .asSeconds())));
            text.setPosition(
                { (INTERNAL_GAME_RESOLUTION.x - text.getGlobalBounds().size.x)
                      / 2.f,
                  10.f });
            window.draw(text);
        }
        else
        {
            text.setString(
                uni::format("Wave: {}", scene.survivalSpawnerContext.wave));
            text.setPosition(
                { (INTERNAL_GAME_RESOLUTION.x - text.getGlobalBounds().size.x)
                      / 2.f,
                  10.f });
            window.draw(text);

            text.setString(uni::format(
                "{} / {}",
                scene.survivalSpawnerContext.enemiesKilledInCurrentWave,
                scene.survivalSpawnerContext.enemiesInCurrentWave));
            text.setPosition(
                { (INTERNAL_GAME_RESOLUTION.x - text.getGlobalBounds().size.x)
                      / 2.f,
                  40.f });
            window.draw(text);
        }
    }

    if (scene.hudMessage.displayTime > sf::Time::Zero)
    {
        text.setString(scene.hudMessage.text);
        text.setPosition(
            { (INTERNAL_GAME_RESOLUTION.x - text.getGlobalBounds().size.x)
                  / 2.f,
              INTERNAL_GAME_RESOLUTION.y * 2.f / 3.f });
        window.draw(text);
    }
}

void RenderingEngine::renderTouchControls(dgm::Window& window)
{
    auto& model = touchController.getTouchModel();
    dgm::Circle thumb({ 0.f, 0.f }, 16.f);

    for (auto& object : model.objects)
    {
        object.touchArea.debugRender(window, sf::Color(128, 128, 128, 128));
        if (object.kind == TouchObjectKind::Joystick)
        {
            thumb.setPosition(object.touchPosition);
            thumb.debugRender(window, sf::Color(128, 128, 128));
        }
    }
}

std::vector<Face> RenderingEngine::getActorFaces() const
{
    std::vector<Face> faces;

    for (auto&& [entity, collider, body, skin] :
         scene.actors.view<Collider, PhysicsBody, Skin>().each())
    {
        bool flipX = false;
        if (auto lookDirection = scene.actors.try_get<LookDirection>(entity))
        {
            flipX = lookDirection->get().dot(sf::Vector2f(1.f, 0.f)) >= 0.f;
        }

        const auto position = std::visit(
            overloads {
                [&](const dgm::Circle& c) -> std::optional<sf::Vector2f>
                {
                    if (!worldCamera.isObjectVisible(c)) return std::nullopt;
                    return c.getPosition()
                           + skin.spriteOriginOffsetFromCollider;
                },
                [&](const dgm::Rect& r) -> std::optional<sf::Vector2f>
                {
                    if (!worldCamera.isObjectVisible(r)) return std::nullopt;
                    return r.getCenter() + skin.spriteOriginOffsetFromCollider;
                },
            },
            collider.shape);

        if (position)
        {
            faces.push_back(Face {
                .origin = *position,
                .texUvs = getFrame(
                    skin.skinType,
                    skin.animation.getStateName(),
                    skin.animation.getCurrentFrameIndex()),
                .scale =
                    sf::Vector2f {
                        flipX ? -1.f : 1.f,
                        1.f,
                    }
                    * skin.scale,
                .rotation = skin.kind == ActorKind::Projectile
                                ? body.forward.angle()
                                : sf::degrees(0),
            });
        }
    }

    std::ranges::sort(faces, std::less<Face> {});

    return faces;
}
