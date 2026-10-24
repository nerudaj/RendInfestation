#include "appstate/AppStateSurvivalLevelSelect.hpp"
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
        dic.guiBuilderFactory.createSimpleLayoutBuilder()
            .withNoBackground()
            .withPlainTitle(StringId::SurvivalSetupTitle, HeadingLevel::H1)
            .withContent(buildContent())
            .withBottomLeftButton(StringId::Back, [&] { onBack(); })
            .withBottomRightButton(StringId::PlayButton, [&] { onPlay(); })
            .build());
}

tgui::Container::Ptr AppStateSurvivalLevelSelect::buildContent()
{
    auto&& content = tgui::HorizontalLayout::create();

    content->add(buildMapSelectLayout());
    content->addSpace(0.05f);
    content->add(buildBoonSelectLayout());

    return content;
}

tgui::Container::Ptr AppStateSurvivalLevelSelect::buildMapSelectLayout()
{
    auto&& levelSelectPanel = tgui::Panel::create();
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
    return levelSelectPanel;
}

tgui::Container::Ptr AppStateSurvivalLevelSelect::buildBoonSelectLayout()
{
    auto&& boonSelectPanel = tgui::ScrollablePanel::create();
    boonSelectPanel->getRenderer()->setPadding({ 20.f, 20.f });

    auto&& vbox = tgui::GrowVerticalLayout::create();
    boonSelectPanel->add(vbox);

    auto&& titleRow = WidgetBuilder::createRow(dic.sizer);
    titleRow->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(StringId::GameSetup), dic.sizer, "justify"_true));
    vbox->add(titleRow);

    auto&& makeOption = [&](StringId labelId, tgui::Widget::Ptr widget)
    {
        auto&& hbox = tgui::HorizontalLayout::create(
            { "100%", dic.sizer.getBaseContainerHeight() });

        hbox->add(
            WidgetBuilder::createTextLabel(
                dic.strings.getString(labelId), dic.sizer),
            0.8f);

        hbox->add(widget, 0.2f);

        return hbox;
    };

    auto&& saveMgr = dic.saveManager;

    vbox->add(makeOption(
        saveMgr.isWave4StartUnlocked() ? StringId::BoonWave4
                                       : StringId::BoonLocked,
        WidgetBuilder::createCheckbox(
            false,
            [&](bool value) { gameProps.boons.wave4start = value; },
            WidgetOptions {
                .enabled = saveMgr.isWave4StartUnlocked(),
            })));

    vbox->add(makeOption(
        saveMgr.isExtraWeaponUnlocked() ? StringId::BoonExtraWeapon
                                        : StringId::BoonLocked,
        WidgetBuilder::createCheckbox(
            false,
            [&](bool value) { gameProps.boons.extraWeapon = value; },
            WidgetOptions {
                .enabled = saveMgr.isExtraWeaponUnlocked(),
            })));

    auto&& weaponSelector = WidgetBuilder::createCarousel(
        saveMgr.getUnlockedCombos().size(),
        [&](tgui::Container::Ptr parent, size_t pageIdx)
        {
            parent->removeAllWidgets();
            auto&& combos =
                saveMgr.getUnlockedCombos() | std::ranges::to<std::vector>();
            parent->add(WidgetBuilder::createTextLabel(
                dic.strings.getString(getWeaponComboName(combos[pageIdx])),
                dic.sizer));
            gameProps.boons.startingWeapon = static_cast<WeaponCombo>(pageIdx);
        },
        dic.sizer,
        dic.soundPlayer);
    weaponSelector->setSize({ "100%", dic.sizer.getBaseContainerHeight() * 2 });
    vbox->add(weaponSelector);

    return boonSelectPanel;
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
