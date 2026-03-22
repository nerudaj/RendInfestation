#include "game/engine/RenderingEngine.hpp"
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
    , worldCamera(createFullscreenCamera(
          sf::Vector2f(settings.video.resolution), INTERNAL_GAME_RESOLUTION))
    , hudCamera(
          sf::FloatRect { { 0.f, 0.f }, { 1.f, 1.f } },
          sf::Vector2f(settings.video.resolution))
    , text(resmgr.get<sf::Font>("ChunkFive-Regular.ttf"))
    , pipeline(atlas.atlas.getTexture())
    , lightPipeline(atlas.atlas.getTexture(), sf::BlendAdd)
    , tilesClip(atlas.atlas.getClip(atlas.tilesLocation))
    , cameraPosition(
          scene.actors.get<Collider>(scene.playerEntity).getPosition())
{
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
        pipeline.addFace(face.origin, face.texUvs, sf::degrees(0), face.scale);

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
    return;

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
    text.setPosition({ 10.f, 10.f });
    text.setString(fpsCounter.getText());
    window.draw(text);

    text.setPosition({ 10.f, 50.f });
    const int health = scene.actors.get<Health>(scene.playerEntity).get();
    text.setString(uni::format("Player health: {}", health));
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
                { (window.getSize().x - text.getGlobalBounds().size.x) / 2.f,
                  10.f });
            window.draw(text);
        }
        else
        {
            text.setString(
                uni::format("Wave: {}", scene.survivalSpawnerContext.wave));
            text.setPosition(
                { (window.getSize().x - text.getGlobalBounds().size.x) / 2.f,
                  10.f });
            window.draw(text);

            text.setString(uni::format(
                "{} / {}",
                scene.survivalSpawnerContext.enemiesKilledInCurrentWave,
                scene.survivalSpawnerContext.enemiesInCurrentWave));
            text.setPosition(
                { (window.getSize().x - text.getGlobalBounds().size.x) / 2.f,
                  40.f });
            window.draw(text);
        }
    }

    if (scene.hudMessage.displayTime > sf::Time::Zero)
    {
        text.setString(scene.hudMessage.text);
        text.setPosition(
            { (window.getSize().x - text.getGlobalBounds().size.x) / 2.f,
              window.getSize().y * 2.f / 3.f });
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

    for (auto&& [entity, collider, skin] :
         scene.actors.view<Collider, Skin>().each())
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
                    },
            });
        }
    }

    std::ranges::sort(faces, std::less<Face> {});

    return faces;
}
