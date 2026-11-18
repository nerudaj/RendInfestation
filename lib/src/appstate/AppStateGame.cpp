#include "appstate/AppStateGame.hpp"
#include "appstate/AppStateNoteReader.hpp"
#include "appstate/AppStatePause.hpp"
#include "appstate/AppStateSurvivalGameOver.hpp"
#include "appstate/AppStateTransitions.hpp"
#include "appstate/AppStateWeaponModification.hpp"
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
            dic.jukebox.pause();
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
    scene.status.gameDuration += app.time.getElapsed();

    systemExecutor.update(app.time);

    aiEngine.update(app.time);

    gameEvents.processEvents(
        aiEngine,
        systemExecutor,
        janitor,
        overloads { [&](event::WorkbenchOpened&)
                    { app.pushState<AppStateWeaponModification>(dic, scene); },
                    [&](event::NoteOpened& e)
                    { app.pushState<AppStateNoteReader>(dic, e.noteId); },
                    [&](event::WaveEnded&)
                    {
                        app.pushState<AppStateChooseBonus>(
                            dic, scene, gameProps.boons.allowGoldModules);
                    },
                    [&](auto&&) {} });
    janitor.cleanScene(scene);

    if (scene.status.finished && AppState::hasFocus)
    {
        AppStateTransitions::applyFadeOutThenInTransition<
            AppStateSurvivalGameOver>(
            app,
            FadeOutOptions(FadeOptions {
                .duration = sf::seconds(2.f),
                .shouldSimulateUnderlyingState = true,
            }),
            FadeInOptions(FadeOptions {
                .duration = sf::seconds(1.f),
            }),
            dic,
            scene.status,
            gameProps.mapIdx);
    }
}

void AppStateGame::draw()
{
    systemExecutor.get<RenderingEngine>().draw(app.window);
}

void AppStateGame::restoreFocusImpl(const std::string& msg)
{
    if (auto message = Messaging::deserialize(msg))
    {
        std::visit(
            overloads {
                [&](PopIfNotMenu&) { app.popState(msg); },
                [&](PopIfNotGame&) {},
                [&](RestartGame&) { app.popState(msg); },
            },
            *message);
    }

    // "If this fails, you've probably decided to add back campaign without "
    // "refactoring hard-coded survival bits"
    assert(scene.survivalGameDirector);
    const int wave = scene.survivalGameDirector->getContext().wave;

    boonUnlockEvaluator.evaluateUnlocks(
        wave,
        gameProps.mapIdx,
        scene.playerInventory,
        [&](StringId msg)
        { systemExecutor.get<GameRulesEngine>().postMessage(msg); });

    // I don't care anymore about placement of this code:
    if (wave > 14) aiEngine.setAttackTicketCount(HARD_ATTACK_TICKET_COUNT);

    sf::Mouse::setPosition(
        sf::Vector2i(app.window.getSize() / 2u),
        app.window.getSfmlWindowContext());
}
