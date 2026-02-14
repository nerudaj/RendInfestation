#include "game/engine/RenderingEngine.hpp"

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
    , tilesClip(atlas.atlas.getClip(atlas.tilesLocation))
{
}

void RenderingEngine::update(const dgm::Time& time)
{
    fpsCounter.update(time.getDeltaTime());
    timeElapsed += time.getDeltaTime();
    worldCamera.setPosition(scene.cameraPosition);
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
        }
    }

    for (auto&& [actor, _] : scene.actors)
    {
        std::visit(
            overloads {
                [&](const dgm::Circle& c)
                {
                    pipeline.addFace(
                        c.getPosition(),
                        sf::FloatRect(actor.animation.getCurrentFrame()));
                    c.debugRender(window);
                },
                [&](const dgm::Rect& r)
                {
                    pipeline.addFace(
                        r.getCenter(),
                        sf::FloatRect(actor.animation.getCurrentFrame()));
                    r.debugRender(window);
                },
            },
            actor.body);
    }

    pipeline.renderTo(window);

    /*
    for (auto&& [actor, _] : scene.actors)
    {
        std::visit(
            overloads {
                [&](const dgm::Circle& c) { c.debugRender(window); },
                [&](const dgm::Rect& r) { r.debugRender(window); },
            },
            actor.body);
    }*/
}

void RenderingEngine::renderHud(dgm::Window& window)
{
    text.setPosition({ 10.f, 10.f });
    text.setString(fpsCounter.getText());
    window.draw(text);
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
