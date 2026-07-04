#include "appstate/Game/engine/RenderingEngine.hpp"
#include "appstate/Game/definitions/Constants.hpp"
#include "appstate/Game/enums/Hud.hpp"
#include "gui/Icon.hpp"
#include "rendering/CameraFactory.hpp"
#include <cmath>

RenderingEngine::RenderingEngine(
    dgm::ResourceManager& resmgr,
    GameScene& scene,
    const GameTextureAtlas& atlas,
    const AppSettings& settings,
    const TouchController& touchController,
    const StringProvider<StringId>& strings) noexcept
    : scene(scene)
    , atlas(atlas)
    , settings(settings)
    , touchController(touchController)
    , strings(strings)
    , resmgr(resmgr)
    , worldCamera(CameraFactory::createFullscreenCamera(
          sf::Vector2f(settings.video.resolution), INTERNAL_GAME_RESOLUTION))
    , hudCamera(
          sf::FloatRect { { 0.f, 0.f }, { 1.f, 1.f } },
          INTERNAL_GAME_RESOLUTION)
    , touchInputCamera(
          sf::FloatRect { { 0.f, 0.f }, { 1.f, 1.f } },
          sf::Vector2f(settings.video.resolution))
    , viewportCollider(
          { 0.f, 0.f },
          INTERNAL_GAME_RESOLUTION + sf::Vector2f({ 128.f, 128.f }))
    , text(resmgr.get<sf::Font>(FONT_NAME))
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
    resmgr.getMutable<sf::Font>(FONT_NAME).setSmooth(false);
    setCameraPosition(
        scene.actors.get<Collider>(scene.playerEntity).getPosition());
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

    setCameraPosition(cameraPosition);
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

void RenderingEngine::renderWorld(dgm::Window& window)
{
    pipeline.clear();

    addLevelFacesToPipeline();

    for (auto&& face : getActorFaces())
        pipeline.addFace(
            { face.origin.x, face.origin.y },
            face.texUvs,
            face.rotation,
            face.scale);

    // Render crosshair
    const auto playerPosition =
        scene.actors.get<Collider>(scene.playerEntity).getPosition();

    pipeline.addFace(
        playerPosition
            + scene.actors.get<LookDirection>(scene.playerEntity).get(),
        sf::FloatRect {
            atlas.atlas.getClip(atlas.crosshairsLocation).getFrame(0) });

    addParticlesToPipeline();

    pipeline.renderTo(window);

    renderLights(window);

    // Render interact prompt
    if (scene.interactionTrigger)
    {
        text.setString(strings.getString(StringId::Interact));
        text.setPosition(
            playerPosition - text.getGlobalBounds().size / 2.f
            - sf::Vector2f(0.f, 25.f));
        window.draw(text);
    }

    if (settings.video.renderColliders) renderColliders(window);
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
            if (!isObjectVisible(dgm::Circle(pos, 16.f))) continue;

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

void RenderingEngine::addParticlesToPipeline()
{
    for (auto&& [entity, system] : scene.actors.view<ParticleSystem>().each())
    {
        for (auto&& particle : system.particles)
        {
            // if particles stop rendering then it is because atlas texture
            // clanged and the white pixel is no longer at the same coords.

            pipeline.addFace(
                particle.position,
                sf::FloatRect {
                    { 197.f, 198.f },
                    { 1.f, 1.f },
                },
                sf::degrees(0.f),
                particle.size,
                particle.color);
        }
    }
}

void RenderingEngine::renderColliders(dgm::Window& window)
{
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

    renderHudStrings(window);
}

void RenderingEngine::renderHudBackgroundAndHealth(
    const sf::Vector2f& hudOrigin, dgm::Window& window)
{
    hudSprite.setTextureRect(hudClip.getFrame(Hud::Background));
    hudSprite.setPosition(hudOrigin);
    window.draw(hudSprite);

    const int health = std::clamp(
        scene.actors.get<Health>(scene.playerEntity).value,
        0,
        scene.playerMaxHealth);
    const auto tenth = (10 * (9 + health)) / scene.playerMaxHealth;
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
    const auto& activeWeaponConfig =
        scene.loadout.weapons[weaponInventory.activeWeapon];
    for (auto&& [idx, module] :
         std::views::enumerate(activeWeaponConfig.modules))
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
    text.setCharacterSize(FONT_BASE_HEIGHT);
#ifdef _DEBUG
    text.setPosition({ 10.f, 2.f });
    text.setString(fpsCounter.getText());
    window.draw(text);
#endif

    if (scene.survivalGameDirector)
    {
        auto& context = scene.survivalGameDirector->getContext();

        if (context.state == SurvivalModeState::WaitingForNextWave)
        {
            text.setString(uni::format(
                "Next wave in: {}s",
                std::ceil(context.timeTillNextWave.asSeconds())));
            text.setPosition(
                { (INTERNAL_GAME_RESOLUTION.x - text.getGlobalBounds().size.x)
                      / 2.f,
                  2.f });
            window.draw(text);
        }
        else
        {
            text.setString(uni::format("Wave: {}", context.wave));
            text.setPosition(
                { (INTERNAL_GAME_RESOLUTION.x - text.getGlobalBounds().size.x)
                      / 2.f,
                  2.f });
            window.draw(text);

            text.setString(uni::format(
                "{} / {}",
                context.enemiesKilledInCurrentWave,
                context.enemiesInCurrentWave));
            text.setPosition(
                { (INTERNAL_GAME_RESOLUTION.x - text.getGlobalBounds().size.x)
                      / 2.f,
                  11.f });
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

    // First two objects are joysticks
    for (auto&& object : model.objects | uni::views::take(2))
    {
        object.touchArea.debugRender(window, sf::Color(128, 128, 128, 128));
        thumb.setPosition(object.touchPosition);
        thumb.debugRender(window, sf::Color(128, 128, 128));
    }

    const auto buttonRadius = model.objects[2].touchArea.getRadius() * 0.8f;

    const auto offset = sf::Vector2f { buttonRadius, buttonRadius };
    const auto scale = 2 * buttonRadius / iconsClip.getFrameSize().x;
    hudSprite.setScale({ scale, scale });

    // Third is pause
    hudSprite.setTextureRect(iconsClip.getFrame(Icon::Pause2));
    hudSprite.setPosition(model.objects[2].touchArea.getPosition() - offset);
    window.draw(hudSprite);

    // Fourth is swap
    if (scene.loadout.weapons.size() > 1)
    {
        model.objects[3].touchArea.debugRender(
            window, sf::Color(128, 128, 128, 128));
        hudSprite.setTextureRect(iconsClip.getFrame(Icon::Swap));
        hudSprite.setPosition(
            model.objects[3].touchArea.getPosition() - offset);
        window.draw(hudSprite);
    }

    // Fifth is interact
    if (scene.interactionTrigger)
    {
        model.objects[4].touchArea.debugRender(
            window, sf::Color(128, 128, 128, 128));
        hudSprite.setTextureRect(iconsClip.getFrame(Icon::Hand));
        hudSprite.setPosition(
            model.objects[4].touchArea.getPosition() - offset);
        window.draw(hudSprite);
    }

    hudSprite.setScale({ 1.f, 1.f });
}

sf::Angle
RenderingEngine::getEntityRotation(entt::entity entity, EntityKind kind) const
{
    if (kind != EntityKind::Projectile) return sf::degrees(0);
    const auto forward = scene.actors.get<PhysicsBody>(entity).forward;
    return forward.lengthSquared() > 0.f ? forward.angle() : sf::degrees(0);
}

std::vector<Face> RenderingEngine::getActorFaces() const
{
    std::vector<Face> faces;

    for (auto&& [entity, collider, skin, zIndex] :
         scene.actors.view<Collider, Skin, ZIndex>().each())
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
                    if (!isObjectVisible(c)) return std::nullopt;
                    return c.getPosition();
                },
                [&](const dgm::Rect& r) -> std::optional<sf::Vector2f>
                {
                    if (!isObjectVisible(r)) return std::nullopt;
                    return r.getCenter();
                },
            },
            collider.shape);

        if (position)
        {
            faces.push_back(Face {
                .origin = *position + skin.spriteOriginOffsetFromCollider,
                .sort =
                    SortCoords {
                        .y = position->y,
                        .z = static_cast<float>(zIndex.get()),
                    },
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
                .rotation = getEntityRotation(entity, skin.kind),
            });
        }
    }

    std::ranges::sort(faces, std::less<Face> {});

    return faces;
}

void RenderingEngine::setCameraPosition(const sf::Vector2f& position)
{
    worldCamera.setPosition(position);
    viewportCollider.setPosition(position - viewportCollider.getSize() / 2.f);
}
