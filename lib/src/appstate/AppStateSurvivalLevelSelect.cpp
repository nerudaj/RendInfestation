#include "appstate/AppStateSurvivalLevelSelect.hpp"
#include "appstate/AppStateGame.hpp"
#include "appstate/AppStateLoading.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "gui/TguiHelper.hpp"
#include "gui/builders/WidgetBuilder.hpp"
#include "strings/StringId.hpp"

constexpr const char* PLAY_BUTTON_ID = "PlayButton";

AppStateSurvivalLevelSelect::AppStateSurvivalLevelSelect(
    dgm::App& app, DependencyContainer& dic)
    : dgm::AppState(app), dic(dic)
{
    buildLayout();
    assert(LEVELS.size() == MAP_LIMIT);
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
    auto msg = Messaging::deserialize(message);
    if (msg)
    {
        std::visit(
            overloads {
                [&](const RestartGame&) { onPlay(); },
                [&](const auto&) { app.popState(message); },
            },
            *msg);
    }
}

void AppStateSurvivalLevelSelect::buildLayout()
{
    dic.gui.rebuildWith(
        dic.guiBuilderFactory.createSimpleLayoutBuilder()
            .withNoBackground()
            .withPlainTitle(StringId::SurvivalSetupTitle, HeadingLevel::H1)
            .withContent(buildContent())
            .withBottomLeftButton(StringId::Back, [&] { onBack(); })
            .withBottomRightButton(
                StringId::PlayButton,
                [&] { onPlay(); },
                WidgetOptions {
                    .id = PLAY_BUTTON_ID,
                })
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
        // Hide bonus map until unlocked
        dic.saveManager.isNthSurvivalMapUnlocked(3) ? LEVELS.size() : 3,
        [this](tgui::Container::Ptr parent, size_t page)
        {
            onMapSelected(page);
            buildMapDetailLayout(parent, page);
        },
        dic.sizer,
        dic.soundPlayer));
    return levelSelectPanel;
}

tgui::Container::Ptr AppStateSurvivalLevelSelect::buildBoonSelectLayout()
{
    auto&& saveMgr = dic.saveManager;

    auto&& unlockedCombosLabels =
        saveMgr.getUnlockedCombos()
        | std::views::transform(
            [&](WeaponCombo combo)
            {
                return std::string(
                    dic.strings.getString(getWeaponComboName(combo)));
            })
        | std::ranges::to<std::vector>();

    auto&& boonSelectPanel =
        WidgetBuilder::createContainer<tgui::ScrollablePanel>(dic.sizer);
    boonSelectPanel->getRenderer()->setPadding({ 20.f, 20.f });

    boonSelectPanel->add(
        dic.guiBuilderFactory.createFormBuilder()
            .addLabel(StringId::GameSetup)
            .addOption(
                saveMgr.isWave4StartUnlocked() ? StringId::BoonWave4
                                               : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.wave4start = value; },
                    WidgetOptions {
                        .enabled = saveMgr.isWave4StartUnlocked(),
                    }))
            .addOption(
                saveMgr.isExtraWeaponUnlocked() ? StringId::BoonExtraWeapon
                                                : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.extraWeapon = value; },
                    WidgetOptions {
                        .enabled = saveMgr.isExtraWeaponUnlocked(),
                    }))
            .addOption(
                saveMgr.isReviveUnlocked() ? StringId::BoonEmergencyRevive
                                           : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.revive = value; },
                    WidgetOptions {
                        .enabled = saveMgr.isReviveUnlocked(),
                    }))
            .addOption(
                saveMgr.areGoldModulesUnlocked() ? StringId::BoonGoldModules
                                                 : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value)
                    { gameProps.boons.allowGoldModules = value; },
                    WidgetOptions {
                        .enabled = saveMgr.areGoldModulesUnlocked(),
                    }))
            .addOption(
                saveMgr.isHardModeUnlocked() ? StringId::BoonHardMode
                                             : StringId::BoonLocked,
                WidgetBuilder::createCheckbox(
                    false,
                    [&](bool value) { gameProps.boons.hardMode = value; },
                    WidgetOptions {
                        .enabled = saveMgr.isHardModeUnlocked(),
                    }))
            //.addGap()
            .addLabel(StringId::StartingWeapon)
            .addFullRowOption(WidgetBuilder::createListChoice(
                unlockedCombosLabels,
                0u,
                [&](size_t pageIdx)
                {
                    auto&& combos = saveMgr.getUnlockedCombos()
                                    | std::ranges::to<std::vector>();
                    gameProps.boons.startingWeapon = combos.at(pageIdx);
                },
                dic.sizer,
                dic.soundPlayer))
            .build());

    return boonSelectPanel;
}

void AppStateSurvivalLevelSelect::buildMapDetailLayout(
    tgui::Container::Ptr parent, size_t mapIdx)
{
    parent->removeAllWidgets();
    auto&& group = tgui::Group::create();

    auto&& labelRow = WidgetBuilder::createRow(dic.sizer);
    labelRow->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(LEVELS.at(mapIdx).levelName),
        dic.sizer,
        "justify"_true));
    group->add(labelRow, "MapNameRow");

    auto&& imageGroup =
        tgui::Group::create({ "100%", "100% - MapNameRow.height" });
    TguiHelper::alignInParent(
        imageGroup,
        tgui::HorizontalAlignment::Left,
        tgui::VerticalAlignment::Bottom);

    const bool mapUnlocked = dic.saveManager.isNthSurvivalMapUnlocked(mapIdx);
    if (mapUnlocked)
    {
        auto vbox = tgui::Group::create();
        imageGroup->add(vbox);

        auto&& bestScoreLabelWrapper =
            tgui::Group::create({ "100%", dic.sizer.getBaseContainerHeight() });
        bestScoreLabelWrapper->add(WidgetBuilder::createTextLabel(
            uni::format(
                "{}: {}",
                dic.strings.getString(StringId::BestScore),
                dic.saveManager.getBestScore(mapIdx)),
            dic.sizer,
            "justify"_true));
        vbox->add(bestScoreLabelWrapper, "BestScoreLabel");

        auto&& heroImagePanel = tgui::Panel::create(
            { "height", "(100% - BestScoreLabel.height) * 0.9" });
        heroImagePanel->setPosition(
            { "50% - width / 2", "BestScoreLabel.height" });
        heroImagePanel->getRenderer()->setTextureBackground(
            dic.resmgr.get<tgui::Texture>(LEVELS.at(mapIdx).imageName));
        vbox->add(heroImagePanel);
    }
    else
    {
        imageGroup->add(WidgetBuilder::createTextLabel(
            dic.strings.getString(LEVELS.at(mapIdx).levelLockedString),
            dic.sizer,
            "justify"_true));
    }

    group->add(imageGroup);

    parent->add(group);
}

void AppStateSurvivalLevelSelect::onMapSelected(size_t mapIdx)
{
    const bool mapUnlocked = dic.saveManager.isNthSurvivalMapUnlocked(mapIdx);

    auto&& playBtn = dic.gui.get<tgui::Button>(PLAY_BUTTON_ID);
    if (!playBtn)
        return; // happens on the first pass through builders when PlayButton
                // doesn't yet exist

    WidgetBuilder::setButtonEnabled(playBtn, mapUnlocked);

    if (mapUnlocked)
    {
        gameProps.mapIdx = mapIdx;
        gameProps.mapName = LEVELS.at(mapIdx).fileId;
    }
}

void AppStateSurvivalLevelSelect::onBack()
{
    app.popState();
}

void AppStateSurvivalLevelSelect::onPlay()
{
    app.pushState<AppStateLoading>(dic, gameProps);
}
