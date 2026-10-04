#include "appstate/AppStateSurvivalLevelSelect.hpp"
#include "appstate/AppStateDimmer.hpp"
#include "appstate/AppStateGame.hpp"
#include "appstate/AppStateLoading.hpp"
#include "appstate/CommonHandler.hpp"
#include "strings/StringId.hpp"

AppStateSurvivalLevelSelect::AppStateSurvivalLevelSelect(
    dgm::App& app, DependencyContainer& dic)
    : dgm::AppState(app), dic(dic)
{
    buildLayout();
}

void AppStateSurvivalLevelSelect::input()
{
    CommonHandler::handleInput(
        app, dic, dic.settings.input, CommonHandlerOptions {});
}

void AppStateSurvivalLevelSelect::update() {}

void AppStateSurvivalLevelSelect::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateSurvivalLevelSelect::restoreFocusImpl(const std::string& message)
{
    app.popState(message);
}

void AppStateSurvivalLevelSelect::buildLayout()
{
    dic.gui.rebuildWith(
        dic.guiBuilderFactory.createDefaultLayoutBuilder()
            .withNoBackground()
            .withTitle(StringId::SurvivalLevelSelect, HeadingLevel::H1)
            .withContent(buildContent())
            .withNoTopLeftButton()
            .withNoTopRightButton()
            .withBottomLeftButton(StringId::Back, [&] { onBack(); })
            .withBottomRightButton(StringId::PlayButton, [&] { onPlay(); })
            .build());
}

tgui::Container::Ptr AppStateSurvivalLevelSelect::buildContent()
{
    auto&& content = tgui::HorizontalLayout::create();

    auto&& levelSelectPanel = tgui::Panel::create({ "50%", "100%" });
    auto&& vbox = tgui::GrowVerticalLayout::create();
    vbox->getRenderer()->setPadding({ 10.f, 10.f });
    levelSelectPanel->add(vbox);

    auto&& labelRow = WidgetBuilder::createRow(dic.sizer);
    labelRow->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(StringId::SelectMap), dic.sizer));
    vbox->add(labelRow, "LabelRow");

    auto&& dropdownRow = WidgetBuilder::createRow(dic.sizer);
    dropdownRow->add(WidgetBuilder::createDropdown(
        LEVEL_NAMES,
        LEVEL_NAMES.front(),
        [&](size_t idx) { gameProps.mapName = LEVEL_IDS[idx]; },
        dic.sizer));
    vbox->add(dropdownRow, "DropdownRow");

    auto&& imageBox = tgui::Group::create({ "100%", "width / 16 * 9" });
    auto&& heroImagePanel = tgui::Panel::create({ "16 * height / 9", "80%" });
    TguiHelper::centerInParent(heroImagePanel);
    heroImagePanel->getRenderer()->setTextureBackground(
        dic.resmgr.get<tgui::Texture>("placeholder16_9.png"));
    imageBox->add(heroImagePanel);
    vbox->add(imageBox);

    auto&& boonSelectPanel = tgui::Panel::create({ "50%", "100%" });
    boonSelectPanel->add(
        dic.guiBuilderFactory.createFormBuilder()
            .addLabel(StringId::SelectBoon)
            .addOption(
                StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.wave4start = value; },
                    WidgetOptions { .enabled = false }))
            .addOption(
                StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.wave4start = value; },
                    WidgetOptions { .enabled = false }))
            .addOption(
                StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.extraWeapon = value; },
                    WidgetOptions { .enabled = false }))
            .build());

    content->add(levelSelectPanel);
    content->addSpace(0.05f);
    content->add(boonSelectPanel);

    return content;
}

void AppStateSurvivalLevelSelect::onBack()
{
    app.popState();
}

void AppStateSurvivalLevelSelect::onPlay()
{
    app.pushState<AppStateLoading>(dic, gameProps);
}
