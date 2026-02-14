#include "appstate/AppStateGame.hpp"
#include "appstate/AppStatePause.hpp"
#include "appstate/Messaging.hpp"

void AppStateGame::input()
{
    if (dic.input.isPauseButtonPressed())
    {
        app.pushState<AppStatePause>(dic);
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
        else
        {
            dic.touchController.processEvent(event);
        }
    }
}

void AppStateGame::update()
{
    gameRulesEngine.update(app.time);
    renderingEngine.update(app.time);

    // Any number of visitors can be passed down for event processing
    gameEvents.processEvents(
        gameRulesEngine,
        [&](const DummyGameEvent&)
        { dic.soundPlayer.playPovSound(SoundId::Land); });
}

void AppStateGame::draw()
{
    renderingEngine.draw(app.window);
}

void AppStateGame::restoreFocusImpl(const std::string& msg)
{
    if (auto message = Messaging::deserialize(msg))
    {
        std::visit([&](PopIfNotMenu&) { app.popState(msg); }, *message);
    }
}
