#include "appstate/AppStateGame.hpp"
#include "appstate/AppStatePause.hpp"
#include "appstate/AppStateSurvivalGameOver.hpp"
#include "appstate/Messaging.hpp"

void AppStateGame::input()
{
    if (dic.input.isPauseButtonPressed())
    {
        app.pushState<AppStatePause>(dic, scene);
        // If pause button is the same as back button, we need
        // to release it to prevent pause menu from immediately closing again
        dic.input.forceRelease(InputKind::BackButton);
    }

    while (const auto event = app.window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            app.exit();
        }
        else if (event->is<sf::Event::FocusGained>())
            dic.jukebox.resume();
        else if (event->is<sf::Event::FocusLost>())
            dic.jukebox.stop();
        else if (event->is<sf::Event::KeyPressed>())
        {
            if (event->getIf<sf::Event::KeyPressed>()->code
                == sf::Keyboard::Key::P)
            {
                std::ignore = app.window.getScreenshot().saveToFile("ss.png");
            }
        }
        else
        {
            dic.touchController.processEvent(event);
        }
    }
}

void AppStateGame::update()
{
    aiEngine.update(app.time);
    gameRulesEngine.update(app.time);
    animationEngine.update(app.time);
    physicsEngine.update(app.time);
    particleEngine.update(app.time);
    renderingEngine.update(app.time);

    gameEvents.processEvents(
        gameRulesEngine, animationEngine, physicsEngine, janitor);
    janitor.cleanScene(scene);

    ++scene.tick;

    if (scene.status.finished)
    {
        app.pushState<AppStateSurvivalGameOver>(dic, scene.status);
    }
}

void AppStateGame::draw()
{
    renderingEngine.draw(app.window);
}

void AppStateGame::restoreFocusImpl(const std::string& msg)
{
    if (auto message = Messaging::deserialize(msg))
    {
        std::visit(
            overloads {
                [&](PopIfNotMenu&) { app.popState(msg); },
                [&](PopIfNotGame&) {},
            },
            *message);
    }

    sf::Mouse::setPosition(
        sf::Vector2i(app.window.getSize() / 2u),
        app.window.getSfmlWindowContext());
}
