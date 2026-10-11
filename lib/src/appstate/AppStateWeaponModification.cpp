#include "appstate/AppStateWeaponModification.hpp"
#include "appstate/AppStateWeaponModuleSelection.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Game/builders/WeaponBuilder.hpp"
#include "appstate/Game/definitions/Components.hpp"
#include "appstate/Messaging.hpp"
#include "gui/GuiBuilderHelper.hpp"
#include "gui/builders/FormBuilder.hpp"
#include "gui/builders/WidgetBuilder.hpp"
#include "rendering/CameraFactory.hpp"
#include "strings/StringId.hpp"
#include "ui-components/SelectablePanelComponent.hpp"
#include <array>

AppStateWeaponModification::AppStateWeaponModification(
    dgm::App& app, DependencyContainer& dic, GameScene& scene)
    : dgm::AppState(
          app,
          {
              .shouldDrawUnderlyingState = true,
          })
    , dic(dic)
    , scene(scene)
{
    buildLayout();
}

void AppStateWeaponModification::input()
{
    CommonHandler::handleInput(app, dic, dic.settings.input);
}

void AppStateWeaponModification::update() {}

void AppStateWeaponModification::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateWeaponModification::buildLayout()
{
    auto&& hbox = tgui::HorizontalLayout::create();

    auto&& weapon1Panel = buildLoadoutPanel(0);
    auto&& weapon2Panel = buildLoadoutPanel(1);

    hbox->add(weapon1Panel);
    hbox->addSpace(0.05f);
    hbox->add(weapon2Panel);

    dic.gui.rebuildWith(
        dic.guiBuilderFactory.createSimpleLayoutBuilder()
            .withNoBackground()
            .withNoTitle()
            .withContent(hbox)
            .withNoBottomLeftButton()
            .withBottomRightButton(StringId::Apply, [&] { onResume(); })
            .build());
}

tgui::Container::Ptr
AppStateWeaponModification::buildLoadoutPanel(size_t loadoutIdx)
{
    const bool locked = scene.loadout.weapons.size() <= loadoutIdx;

    auto&& panel = tgui::Panel::create();
    panel->getRenderer()->setPadding({ 20.f, 20.f });

    auto&& labelRow = WidgetBuilder::createRow(dic.sizer);
    labelRow->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(
            locked            ? StringId::BoonLocked
            : loadoutIdx == 0 ? StringId::Weapon1Title
                              : StringId::Weapon2Title),
        dic.sizer,
        "justify"_true));
    panel->add(labelRow, "WeaponTitle");

    if (locked) return panel;

    // === clear button ===
    auto&& clearButtonGroup = WidgetBuilder::createRow(dic.sizer);
    panel->add(clearButtonGroup, "ClearButton");

    clearButtonGroup->setPosition({ "0%", "100% - height" });

    clearButtonGroup->add(WidgetBuilder::createRowButton(
        dic.strings.getString(StringId::Clear),
        [this, loadoutIdxCopy = loadoutIdx] { onClearLoadout(loadoutIdxCopy); },
        dic.sizer,
        dic.soundPlayer));

    // === module select buttons ===
    auto&& buttonHbox = tgui::HorizontalLayout::create(
        { "100%", 3 * dic.sizer.getBaseContainerHeight() });
    buttonHbox->setPosition({ "0%", "100% - height - ClearButton.height" });

    for (auto idx : std::views::iota(0u, 3u))
    {
        const auto module = scene.loadout.weapons[loadoutIdx].modules[idx];
        buttonHbox->add(buildButtonForSelectingModule(module, idx, loadoutIdx));
    }

    panel->add(buttonHbox, "ModuleButtonsHbox");

    // === image ===
    auto&& imageGroup = tgui::Group::create(
        { "100%",
          "100% - WeaponTitle.height - ModuleButtonsHbox.height - "
          "ClearButton.height" });
    imageGroup->setPosition({ "0%", "WeaponTitle.height" });

    auto&& imagePanel = tgui::Panel::create({ "height * 16 / 9", "80%" });
    TguiHelper::centerInParent(imagePanel);
    imagePanel->getRenderer()->setTextureBackground(
        dic.resmgr.get<tgui::Texture>("placeholder16_9.png"));
    imageGroup->add(imagePanel);

    panel->add(imageGroup);

    return panel;
}

tgui::Container::Ptr AppStateWeaponModification::buildButtonForSelectingModule(
    const WeaponModule module, size_t moduleIdx, size_t loadoutIdx)
{
    auto&& wrapper = tgui::Group::create();
    auto&& selectablePanel = SelectablePanelComponent::buildLayout(dic);
    selectablePanel->onClick(
        [this, loadoutIdxCopy = loadoutIdx, moduleIdxCopy = moduleIdx]
        {
            dic.soundPlayer.playClick();
            selectedLoadoutIdx = loadoutIdxCopy;
            selectedModuleIdx = moduleIdxCopy;
            onModSelected();
        });
    selectablePanel->setSize({ "height", "90%" });
    TguiHelper::centerInParent(selectablePanel);

    auto&& image = tgui::Panel::create();
    image->getRenderer()->setTextureBackground(getWeaponModuleTexture(module));
    image->setSize({ "90%", "90%" });
    TguiHelper::centerInParent(image);
    selectablePanel->add(image);

    wrapper->add(selectablePanel);
    return wrapper;
}

void AppStateWeaponModification::onResume()
{
    scene.updatePlayerLoadout();
    restoreGuiViewport();
    app.popState(Messaging::serialize<PopIfNotGame>());
}

void AppStateWeaponModification::onBack()
{
    restoreGuiViewport();
    app.popState();
}

void AppStateWeaponModification::onModSelected()
{
    app.pushState<AppStateWeaponModuleSelection>(
        std::ref(dic),
        std::cref(scene),
        std::ref(selectedModule),
        scene.loadout.weapons[selectedLoadoutIdx].modules[selectedModuleIdx]);
}

void AppStateWeaponModification::onClearLoadout(size_t loadoutIdx)
{
    assert(scene.loadout.weapons.size() > loadoutIdx);
    for (auto& module : scene.loadout.weapons[loadoutIdx].modules)
        module = WeaponModule::None;
    buildLayout();
}

void AppStateWeaponModification::restoreGuiViewport()
{
    dic.gui.getTguiHandle().setAbsoluteViewport(tgui::FloatRect(
        { 0.f, 0.f }, tgui::Vector2f(sf::Vector2f(app.window.getSize()))));
}

void AppStateWeaponModification::restoreFocusImpl(const std::string&)
{
    if (selectedModule)
    {
        scene.loadout.weapons[selectedLoadoutIdx].modules[selectedModuleIdx] =
            *selectedModule;
        buildLayout();
    }
}
