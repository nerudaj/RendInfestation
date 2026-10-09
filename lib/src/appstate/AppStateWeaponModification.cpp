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
#include <array>

static sf::Vector2f resolutionTo16by9(const sf::Vector2f& resolution)
{
    assert(resolution.x > resolution.y);
    assert(resolution.x / resolution.y >= 16.f / 9.f);

    return sf::Vector2f {
        resolution.y * 16.f / 9.f,
        resolution.y,
    };
}

AppStateWeaponModification::AppStateWeaponModification(
    dgm::App& app, DependencyContainer& dic, GameScene& scene)
    : dgm::AppState(app)
    , dic(dic)
    , scene(scene)
    , renderCamera(CameraFactory::createFullscreenCamera(
          sf::Vector2f(app.window.getSize()), INTERNAL_GAME_RESOLUTION))
    , guiCamera(CameraFactory::createFullscreenCamera(
          sf::Vector2f(app.window.getSize()),
          resolutionTo16by9(sf::Vector2f(app.window.getSize()))))
    , renderer(scene, dic)
    , animationTimer(sf::seconds(0.5f))
{
    buildLayout();

    /*const auto&& windowSize = sf::Vector2f(app.window.getSize());
    auto&& sfmlViewport = guiCamera.getCurrentView().getViewport();
    auto&& tguiViewport = tgui::FloatRect(
        tgui::Vector2f(sfmlViewport.position.componentWiseMul(windowSize)),
        tgui::Vector2f(sfmlViewport.size.componentWiseMul(windowSize)));
    dic.gui.getTguiHandle().setAbsoluteViewport(tguiViewport);*/
}

void AppStateWeaponModification::input()
{
    CommonHandler::handleInput(app, dic, dic.settings.input);
}

void AppStateWeaponModification::update()
{
    //    animationTimer.update(app.time.getElapsed());
}

void AppStateWeaponModification::draw()
{
    //    app.window.setViewFromCamera(renderCamera);
    //    renderer.renderWorkbench(app.window, animationTimer,
    //    currentWeaponIdx);

    // app.window.setViewFromCamera(guiCamera);

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

    auto&& modSelectCallback =
        [this, loadoutIdxCopy = loadoutIdx, moduleIdxCopy = moduleIdx]
    {
        selectedLoadoutIdx = loadoutIdxCopy;
        selectedModuleIdx = moduleIdxCopy;
        onModSelected();
    };

    auto&& button = [&](auto&& callback)
    {
        if (module == WeaponModule::None)
        {
            return WidgetBuilder::createButton(
                "X",
                std::forward<decltype(callback)>(callback),
                dic.sizer,
                dic.soundPlayer);
        }

        return WidgetBuilder::createTexturedButton(
            dic.resmgr.get<tgui::Texture>(
                uni::format("ModuleIcon-{}", std::to_underlying(module))),
            std::forward<decltype(callback)>(callback),
            dic.soundPlayer);
    }(modSelectCallback);

    button->setSize({ "height", "90%" });
    button->setPosition({ "parent.width / 2 - width / 2", "50% - height / 2" });

    wrapper->add(button);

    return wrapper;
}

tgui::Button::Ptr AppStateWeaponModification::createModuleSelectButton(
    WeaponModule module,
    std::function<void(void)>&& callback,
    bool disabled) const
{
    auto&& toLayout = [&](unsigned x, unsigned y)
    {
        return tgui::Layout2d {
            uni::format(
                "{}", app.window.getSize().x * x / INTERNAL_GAME_RESOLUTION.x)
                .c_str(),
            uni::format(
                "{}", app.window.getSize().y * y / INTERNAL_GAME_RESOLUTION.y)
                .c_str(),
        };
    };

    auto&& button =
        tgui::Button::create(module == WeaponModule::None ? "X" : "");

    if (module != WeaponModule::None)
    {
        button->getRenderer()->setTexture(dic.resmgr.get<tgui::Texture>(
            uni::format("ModuleIcon-{}", std::to_underlying(module))));
    }

    if (disabled)
    {
        button->setEnabled(false);
        button->getRenderer()->setOpacity(0.5f);
    }

    button->setSize(toLayout(18, 18));
    button->setPosition(
        { "parent.width / 2 - width / 2", "parent.height / 2 - height  / 2" });
    button->onClick(
        [&, callback = std::move(callback)]
        {
            dic.soundPlayer.playPovSound(SoundId::Crafting);
            callback();
        });

    return button;
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

void AppStateWeaponModification::onCycle()
{
    currentWeaponIdx = (currentWeaponIdx + 1) % 2;
    animationTimer.restart();
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

// Build the subset of available modules (None + unlocked)
/*std::vector<WeaponModule>
AppStateWeaponModification::getAvailableModules() const
{
    std::vector<WeaponModule> modules;
    modules.push_back(WeaponModule::None);
    for (size_t i = 1; i < ALLOWED_MODULES.size(); ++i)
    {
        if (scene.unlockedModules.contains(ALLOWED_MODULES[i]))
            modules.push_back(ALLOWED_MODULES[i]);
    }
    return modules;
}
*/
