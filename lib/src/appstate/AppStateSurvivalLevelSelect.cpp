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
    levelSelectPanel->getRenderer()->setPadding({ 20.f, 20.f });
    levelSelectPanel->add(WidgetBuilder::createCarousel(
        LEVEL_NAMES.size(),
        [this](tgui::Container::Ptr parent, size_t page)
        {
            gameProps.mapName = LEVEL_IDS.at(page);
            buildMapDetailLayout(parent, page);
        },
        dic.sizer,
        dic.soundPlayer));
    /*auto&& vbox = tgui::Group::create();
    vbox->getRenderer()->setPadding({ 10.f, 10.f });
    levelSelectPanel->add(vbox);

    auto&& labelRow = WidgetBuilder::createRow(dic.sizer);
    labelRow->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(StringId::SelectMap), dic.sizer, "justify"_true));
    vbox->add(labelRow, "LabelRow");

    auto&& levelCarousel = WidgetBuilder::createCarousel(
        LEVEL_NAMES.size(),
        [this](tgui::Container::Ptr parent, size_t page)
        {
            gameProps.mapName = LEVEL_IDS.at(page);
            buildMapDetailLayout(parent, page);
        },
        dic.sizer,
        dic.soundPlayer);

    levelCarousel->setSize({ "100%", "parent.innerheight - LabelRow.height" });
    TguiHelper::alignInParent(
        levelCarousel,
        tgui::HorizontalAlignment::Left,
        tgui::VerticalAlignment::Bottom);
    vbox->add(levelCarousel);*/

    auto&& boonSelectPanel = tgui::Panel::create({ "50%", "100%" });
    boonSelectPanel->add(
        dic.guiBuilderFactory.createFormBuilder()
            .addLabel(StringId::SelectBoon)
            .addOption(
                dic.settings.saves.unlockedBoons.wave4start
                    ? StringId::BoonWave4
                    : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.wave4start = value; },
                    WidgetOptions {
                        .enabled = dic.settings.saves.unlockedBoons.wave4start,
                    }))
            .addOption(
                dic.settings.saves.unlockedBoons.wave8start
                    ? StringId::BoonWave8
                    : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.wave8start = value; },
                    WidgetOptions {
                        .enabled = dic.settings.saves.unlockedBoons.wave8start,
                    }))
            .addOption(
                dic.settings.saves.unlockedBoons.extraWeapon
                    ? StringId::BoonExtraWeapon
                    : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.extraWeapon = value; },
                    WidgetOptions {
                        .enabled = dic.settings.saves.unlockedBoons.extraWeapon,
                    }))
            .build());

    content->add(levelSelectPanel);
    content->addSpace(0.05f);
    content->add(boonSelectPanel);

    return content;
}

void AppStateSurvivalLevelSelect::buildMapDetailLayout(
    tgui::Container::Ptr parent, size_t mapIdx)
{
    parent->removeAllWidgets();
    auto&& group = tgui::Group::create();

    auto&& labelRow = WidgetBuilder::createRow(dic.sizer);
    labelRow->add(WidgetBuilder::createTextLabel(
        LEVEL_NAMES.at(mapIdx), dic.sizer, "justify"_true));
    group->add(labelRow, "MapNameRow");

    auto&& imageGroup =
        tgui::Group::create({ "100%", "100% - MapNameRow.height" });
    TguiHelper::alignInParent(
        imageGroup,
        tgui::HorizontalAlignment::Left,
        tgui::VerticalAlignment::Bottom);

    auto&& heroImagePanel = tgui::Panel::create({ "16 * height / 9", "80%" });
    TguiHelper::centerInParent(heroImagePanel);
    heroImagePanel->getRenderer()->setTextureBackground(
        dic.resmgr.get<tgui::Texture>("placeholder16_9.png"));
    imageGroup->add(heroImagePanel);
    group->add(imageGroup);

    parent->add(group);
}

void AppStateSurvivalLevelSelect::onBack()
{
    app.popState();
}

void AppStateSurvivalLevelSelect::onPlay()
{
    app.pushState<AppStateLoading>(dic, gameProps);
}
