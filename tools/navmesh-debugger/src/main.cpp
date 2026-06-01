#include "appstate/Game/builders/GameSceneBuilder.hpp"
#include "appstate/Game/builders/GameTextureAtlasBuilder.hpp"
#include "misc/DependencyContainer.hpp"
#include <DGM/classes/Window.hpp>

int main(int, char**)
{
    auto&& window = dgm::Window(dgm::WindowSettings {
        .resolution = { 1280u, 720u },
        .title = "navmesh-debugger",
        .useFullscreen = false,
    });

    auto&& dic = DependencyContainer(
        window,
        "../assets",
        Language::English,
        AppSettingsStorageModel {},
        "debugger-settings.json");

    auto&& canvas = tgui::CanvasSFML::create();
    auto&& panel = tgui::Panel::create();
    panel->add(canvas);
    // canvas->setScale({ 1.f, -1.f });

    auto&& atlas =
        GameTextureAtlasBuilder::createTextureAtlas(dic.resmgr, { 1024, 1024 });
    auto&& scene = GameSceneBuilder::createScene(
        atlas,
        dic.resmgr,
        dic.input,
        GameModeProperties {
            .mapName = "survival-02.json",
        });

    auto&& tilemap = dgm::TileMap(
        dic.resmgr.get<sf::Texture>("scifi_tiles.png"),
        dic.resmgr.get<dgm::Clip>("scifi_tiles.png.clip"));
    tilemap.build(
        { 32, 32 },
        scene.levelMesh.getRawConstData(),
        scene.levelMesh.getDataSize());

    auto&& startDot = sf::CircleShape(8.f);
    startDot.setFillColor(sf::Color::Yellow);
    startDot.setOrigin({ 8.f, 8.f });

    auto&& endDot = sf::CircleShape(8.f);
    endDot.setFillColor(sf::Color::Blue);
    endDot.setOrigin({ 8.f, 8.f });

    canvas->onClick([&](tgui::Vector2f pos) { startDot.setPosition(pos); });
    canvas->onRightClick([&](tgui::Vector2f pos) { endDot.setPosition(pos); });
    auto&& navmesh = dgm::WorldNavMesh(scene.levelMesh.clone());

    dic.gui.rebuildWith(dic.guiBuilderFactory.createNavbarLayoutBuilder()
                            .withNavbarWidget(WidgetBuilder::createButton(
                                "Open file", [] {}, dic.sizer, dic.soundPlayer))
                            .withContent(panel)
                            .build());

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                std::ignore = window.close();
                break;
            }

            dic.gui.handleEvent(*event);
        }

        window.clear();

        canvas->draw(tilemap);

        for (auto&& [point, connections] : navmesh.jumpPointConnections)
        {
            const auto worldPoint = sf::Vector2f(
                point.componentWiseMul(scene.levelMesh.getVoxelSize()));
            for (auto&& conn : connections)
            {
                const auto worldConnPoint =
                    sf::Vector2f(conn.destination.componentWiseMul(
                        scene.levelMesh.getVoxelSize()));
                sf::Vertex line[] = {
                    sf::Vertex(
                        worldPoint
                            + sf::Vector2f(scene.levelMesh.getVoxelSize())
                                  / 2.f,
                        sf::Color::Red),
                    sf::Vertex(
                        worldConnPoint
                            + sf::Vector2f(scene.levelMesh.getVoxelSize())
                                  / 2.f,
                        sf::Color::Red),
                };
                canvas->draw(line, 2, sf::PrimitiveType::Lines);
            }
        }

        if (startDot.getPosition() != endDot.getPosition())
        {
            auto path = navmesh.computePath(
                startDot.getPosition(), endDot.getPosition());

            auto prevPoint = startDot.getPosition();
            while (!path.isTraversed())
            {
                auto currentPoint = path.getCurrentPoint().coord;
                sf::Vertex line[] = {
                    sf::Vertex(prevPoint, sf::Color::Green),
                    sf::Vertex(currentPoint, sf::Color::Green),
                };

                canvas->draw(line, 2, sf::PrimitiveType::Lines);
                prevPoint = currentPoint;
                path.advance();
            }
        }

        canvas->draw(startDot);
        canvas->draw(endDot);

        canvas->display();
        dic.gui.draw();
        window.display();
    }

    return 0;
}
