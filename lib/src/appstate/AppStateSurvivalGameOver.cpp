#include "appstate/AppStateSurvivalGameOver.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"

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
        builderFactory.createDefaultLayoutBuiler()
            .withNoBackgroundImage()
            .withTitle(
                dic.strings.getString(StringId::GameOver), HeadingLevel::H1)
            .withContent(buildContent())
            .withNoTopLeftButton()
            .withNoTopRightButton()
#ifdef ANDROID
            .withBottomLeftButton(builderFactory.createIconButton(
                Icon::Exit, [&] { onBackToMenu(); }))
#else
            .withBottomLeftButton(builderFactory.createTextButton(
                StringId::BackToMenu, [&] { onBackToMenu(); }))
#endif
            .withNoBottomRightButton()
            .build());
}

tgui::Container::Ptr AppStateSurvivalGameOver::buildContent()
{
    auto panel = tgui::Panel::create();
    auto verticalLayout = tgui::VerticalLayout::create();
    panel->add(verticalLayout);

    verticalLayout->add(WidgetBuilder::createHeading(
        dic.strings.getString(StringId::GameScore),
        dic.sizer,
        HeadingLevel::H2));
    verticalLayout->add(WidgetBuilder::createTextLabel(
        std::to_string(status.score), dic.sizer, "justify"_true));

    return panel;
}

void AppStateSurvivalGameOver::onBackToMenu()
{
    app.popState(Messaging::serialize<PopIfNotMenu>());
}
