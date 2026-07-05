#include "appstate/AppStateSurvivalGameOver.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "gui/Icon.hpp"

AppStateSurvivalGameOver::AppStateSurvivalGameOver(
    dgm::App& app, DependencyContainer& dic, const GameStatus& status) noexcept
    : dgm::AppState(app), dic(dic), status(status)
{
    buildLayout();

    if (dic.saveManager.getBestScore() < status.score)
    {
        dic.saveManager.updateBestScore(status.score);
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
            .withTitleInPanel(
                StringId::GameOver,
                HeadingLevel::H1,
                tgui::HorizontalAlignment::Left)
            .withContent(buildContent())
            .withBottomLeftButton(StringId::Back, [&] { onBackToMenu(); })
            .withNoBottomRightButton()
            .build());
}

tgui::Container::Ptr AppStateSurvivalGameOver::buildContent()
{
    auto panel = tgui::Panel::create();

    const bool isNewBest = dic.saveManager.getBestScore() < status.score;
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
                    std::to_string(dic.saveManager.getBestScore()),
                    dic.sizer,
                    "justify"_true),
            })
            .build());

    return panel;
}

void AppStateSurvivalGameOver::onBackToMenu()
{
    app.popState(Messaging::serialize<PopIfNotMenu>());
}
