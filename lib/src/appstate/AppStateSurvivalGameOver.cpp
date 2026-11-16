#include "appstate/AppStateSurvivalGameOver.hpp"
#include "appstate/AppStateUnlocksSummary.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Game/utility/TimeFormatter.hpp"
#include "appstate/Messaging.hpp"
#include "gui/Icon.hpp"

AppStateSurvivalGameOver::AppStateSurvivalGameOver(
    dgm::App& app,
    DependencyContainer& dic,
    const GameStatus& status,
    const size_t mapIdx) noexcept
    : dgm::AppState(app), dic(dic), status(status), mapIdx(mapIdx)
{
    buildLayout();

    if (dic.saveManager.getBestScore(mapIdx) < status.score)
    {
        dic.saveManager.updateBestScore(mapIdx, status.score);
    }

    // Write all unlocks etc
    dic.saveProgress();

    if (!status.ledger.empty())
    {
        app.pushState<AppStateUnlocksSummary>(dic, status.ledger);
    }
}

void AppStateSurvivalGameOver::input()
{
    CommonHandler::handleInput(
        app,
        dic,
        dic.settings.input,
        {
            .disableGoBack = true,
        });
}

void AppStateSurvivalGameOver::update() {}

void AppStateSurvivalGameOver::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateSurvivalGameOver::buildLayout()
{
    auto& builderFactory = dic.guiBuilderFactory;

    dic.gui.rebuildWith(
        builderFactory.createSimpleLayoutBuilder()
            .withNoBackground()
            .withPlainTitle(
                StringId::GameOver,
                HeadingLevel::H1,
                tgui::HorizontalAlignment::Left)
            .withContent(buildGameSummaryLayout())
            .withBottomLeftButton(StringId::BackToMenu, [&] { onBackToMenu(); })
            .withBottomRightButton(StringId::Restart, [&] { onRestart(); })
            .build());
}

tgui::Container::Ptr AppStateSurvivalGameOver::buildGameSummaryLayout()
{
    auto&& getLayoutSize = []() -> tgui::Layout2d
    {
#ifdef ANDROID
        return tgui::Layout2d { "80%", "100%" };
#else
        return tgui::Layout2d { "60%", "80%" };
#endif
    };

    auto&& panel = WidgetBuilder::createContainer<tgui::Panel>(
        getLayoutSize(), dic.sizer, WidgetOptions {});
    panel->getRenderer()->setPadding({ 20.f, 20.f });
    TguiHelper::centerInParent(panel);

    const bool isNewBest = dic.saveManager.getBestScore(mapIdx) < status.score;
    const auto scoreString = uni::format(
        "{} {}",
        dic.strings.getString(StringId::GameScore),
        isNewBest ? dic.strings.getString(StringId::NewBestScore) : "");

    panel->add(
        dic.guiBuilderFactory.createTableBuilder()
            .withNoHeading()
            .addRow({
                WidgetBuilder::createTextLabel(
                    scoreString, dic.sizer, "justify"_true),
                WidgetBuilder::createTextLabel(
                    std::to_string(status.score), dic.sizer, "justify"_true),
            })
            .addRow({
                WidgetBuilder::createTextLabel(
                    dic.strings.getString(StringId::GameDuration),
                    dic.sizer,
                    "justify"_true),
                WidgetBuilder::createTextLabel(
                    TimeFormatter::toMMSS(status.gameDuration),
                    dic.sizer,
                    "justify"_true),
            })
            .addRow({
                WidgetBuilder::createTextLabel(
                    dic.strings.getString(StringId::EnemiesRouted),
                    dic.sizer,
                    "justify"_true),
                WidgetBuilder::createTextLabel(
                    std::to_string(status.enemiesKilled),
                    dic.sizer,
                    "justify"_true),
            })
            .addRow({
                WidgetBuilder::createTextLabel(
                    dic.strings.getString(StringId::ShotsFired),
                    dic.sizer,
                    "justify"_true),
                WidgetBuilder::createTextLabel(
                    std::to_string(status.shotsFired),
                    dic.sizer,
                    "justify"_true),
            })
            .addRow({
                WidgetBuilder::createTextLabel(
                    dic.strings.getString(StringId::WavesBeaten),
                    dic.sizer,
                    "justify"_true),
                WidgetBuilder::createTextLabel(
                    std::to_string(status.waveBeaten),
                    dic.sizer,
                    "justify"_true),
            })
            .addSeparator()
            .addRow({
                WidgetBuilder::createTextLabel(
                    dic.strings.getString(
                        isNewBest ? StringId::PreviousBestScore
                                  : StringId::CurrentBestScore),
                    dic.sizer,
                    "justify"_true),
                WidgetBuilder::createTextLabel(
                    std::to_string(dic.saveManager.getBestScore(mapIdx)),
                    dic.sizer,
                    "justify"_true),
            })
            .build());

    return panel;
}

void AppStateSurvivalGameOver::onRestart()
{
    app.popState(Messaging::serialize<RestartGame>());
}

void AppStateSurvivalGameOver::onBackToMenu()
{
    app.popState(Messaging::serialize<PopIfNotMenu>());
}
