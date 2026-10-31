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

    // Write all unlocks etc
    dic.saveProgress();
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

    dic.gui.rebuildWith(builderFactory.createSimpleLayoutBuilder()
                            .withNoBackground()
                            .withPlainTitle(
                                StringId::GameOver,
                                HeadingLevel::H1,
                                tgui::HorizontalAlignment::Left)
                            .withContent(buildBentoBoxContent())
                            .withNoBottomButtons()
                            .build());
}

tgui::Container::Ptr AppStateSurvivalGameOver::buildBentoBoxContent()
{
    auto&& vbox = tgui::VerticalLayout::create();

    // 2 rows
    // 1: map icon, game summary
    // 2: unlockables summary, two vertical buttons Restart, Back to Menu

    auto&& firstRowHbox = tgui::HorizontalLayout::create();
    vbox->add(firstRowHbox);

    auto&& mapIconPanel = tgui::Panel::create();
    firstRowHbox->add(mapIconPanel);

    firstRowHbox->addSpace(0.05f);

    auto&& gameSummaryPanel = tgui::ScrollablePanel::create();
    firstRowHbox->add(gameSummaryPanel, 2.f);
    gameSummaryPanel->add(buildGameSummaryLayout());

    vbox->addSpace(0.05f);

    auto&& secondRowHbow = tgui::HorizontalLayout::create();
    vbox->add(secondRowHbow);

    auto&& ledgerPanel = tgui::ScrollablePanel::create();
    ledgerPanel->add(buildLedgerSummaryLayout());
    secondRowHbow->add(ledgerPanel, 2.f);

    secondRowHbow->addSpace(0.05f);

    auto&& buttonVbox = tgui::VerticalLayout::create();
    secondRowHbow->add(buttonVbox);

    buttonVbox->add(WidgetBuilder::createButton(
        dic.strings.getString(StringId::Restart),
        [&] { onRestart(); },
        dic.sizer,
        dic.soundPlayer));

    buttonVbox->addSpace(0.05f);

    buttonVbox->add(WidgetBuilder::createButton(
        dic.strings.getString(StringId::BackToMenu),
        [&] { onBackToMenu(); },
        dic.sizer,
        dic.soundPlayer));

    return vbox;
}

tgui::Container::Ptr AppStateSurvivalGameOver::buildGameSummaryLayout()
{
    const bool isNewBest = dic.saveManager.getBestScore() < status.score;
    const auto scoreString = uni::format(
        "{} {}",
        dic.strings.getString(StringId::GameScore),
        isNewBest ? dic.strings.getString(StringId::NewBestScore) : "");

    return dic.guiBuilderFactory.createTableBuilder()
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
                std::to_string(status.shotsFired), dic.sizer, "justify"_true),
        })
        .addRow({
            WidgetBuilder::createTextLabel(
                dic.strings.getString(StringId::WavesBeaten),
                dic.sizer,
                "justify"_true),
            WidgetBuilder::createTextLabel(
                std::to_string(status.waveBeaten), dic.sizer, "justify"_true),
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
        .build();
}

tgui::Container::Ptr AppStateSurvivalGameOver::buildLedgerSummaryLayout()
{
    auto builder = dic.guiBuilderFactory.createTableBuilder().withNoHeading();

    for (auto&& line : status.ledger)
    {
        builder.addRow({ WidgetBuilder::createTextLabel(
            dic.strings.getString(line), dic.sizer, "justify"_true) });
    }

    return builder.build();
}

void AppStateSurvivalGameOver::onRestart()
{
    app.popState(Messaging::serialize<RestartGame>());
}

void AppStateSurvivalGameOver::onBackToMenu()
{
    app.popState(Messaging::serialize<PopIfNotMenu>());
}
