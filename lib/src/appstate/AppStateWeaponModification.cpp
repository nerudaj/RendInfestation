#include "appstate/AppStateWeaponModification.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "appstate/Game/builders/WeaponBuilder.hpp"
#include "appstate/Game/definitions/Components.hpp"
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
}

void AppStateWeaponModification::input()
{
    CommonHandler::handleInput(app, dic, dic.settings.input);
}

void AppStateWeaponModification::update()
{
    animationTimer.update(app.time.getElapsed());
}

void AppStateWeaponModification::draw()
{
    app.window.setViewFromCamera(renderCamera);
    renderer.renderWorkbench(app.window, animationTimer, currentWeaponIdx);

    app.window.setViewFromCamera(guiCamera);

    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateWeaponModification::buildLayout()
{
    dic.gui.removeAllWidgets();

    auto&& toLayout = [](unsigned x, unsigned y)
    {
        return tgui::Layout2d {
            uni::format("{}%", 100 * x / INTERNAL_GAME_RESOLUTION.x).c_str(),
            uni::format("{}%", 100 * y / INTERNAL_GAME_RESOLUTION.y).c_str(),
        };
    };

    auto&& createButton = [&](const std::string& textureName, auto&& callback)
    {
        auto&& button = tgui::Button::create();

        if (textureName.empty())
        {
            button->getRenderer()->setBackgroundColor(sf::Color::Transparent);
            button->getRenderer()->setBackgroundColorHover(
                sf::Color::Transparent);
        }
        else
        {
            button->getRenderer()->setTexture(
                dic.resmgr.get<tgui::Texture>(textureName + ".png"));
            button->getRenderer()->setTextureHover(
                dic.resmgr.get<tgui::Texture>(textureName + "-hover.png"));
            assert(!button->getRenderer()->getTexture().isSmooth());
        }

        button->getRenderer()->setBorders(0u);
        button->setSize({ "100%", "100%" });
        button->onClick(std::forward<decltype(callback)>(callback));
        return button;
    };

    auto&& resumeButtonLayout = tgui::Group::create();
    resumeButtonLayout->setSize(toLayout(24, 24));
    resumeButtonLayout->setPosition(toLayout(346, 178));
    resumeButtonLayout->add(
        createButton("button-confirm", [&] { onResume(); }));
    dic.gui.add(resumeButtonLayout);

    auto&& cancelButtonLayout = tgui::Group::create();
    cancelButtonLayout->setSize(toLayout(24, 24));
    cancelButtonLayout->setPosition(toLayout(16, 16));
    cancelButtonLayout->add(createButton("button-cancel", [&] { onBack(); }));
    dic.gui.add(cancelButtonLayout);

    auto&& swapWeaponLayout = tgui::Group::create();
    swapWeaponLayout->setSize(toLayout(23, 47));
    swapWeaponLayout->setPosition(toLayout(344, 85));
    swapWeaponLayout->add(
        createButton("button-swap-weapon", [&] { onCycle(); }));
    dic.gui.add(swapWeaponLayout);

    auto&& modSelect1Layout = tgui::Group::create();
    modSelect1Layout->setSize(toLayout(18, 18));
    modSelect1Layout->setPosition(toLayout(35, 163));
    modSelect1Layout->add(createButton("", [&] { onModSelected(0); }));
    dic.gui.add(modSelect1Layout);

    auto&& modSelect2Layout = tgui::Group::create();
    modSelect2Layout->setSize(toLayout(18, 18));
    modSelect2Layout->setPosition(toLayout(58, 163));
    modSelect2Layout->add(createButton("", [&] { onModSelected(1); }));
    dic.gui.add(modSelect2Layout);

    auto&& modSelect3Layout = tgui::Group::create();
    modSelect3Layout->setSize(toLayout(18, 18));
    modSelect3Layout->setPosition(toLayout(81, 163));
    modSelect3Layout->add(createButton("", [&] { onModSelected(2); }));
    dic.gui.add(modSelect3Layout);
}

tgui::ChildWindow::Ptr AppStateWeaponModification::createModuleSelectModal(
    StringId titleStringId, tgui::Layout2d size) const
{
    auto&& modal = tgui::ChildWindow::create(
        dic.strings.getString(titleStringId),
        tgui::ChildWindow::TitleButton::None);
    modal->setSize(size);
    modal->setPosition(
        { "parent.width / 2 - width / 2", "parent.height / 2 - height / 2" });
    modal->getRenderer()->setTitleBarColor(COLOR_PURPLE);
    modal->getRenderer()->setTitleBarHeight(dic.sizer.getBaseContainerHeight());
    modal->getRenderer()->setTextSize(dic.sizer.getBaseFontSize());
    modal->getRenderer()->setBorderColor(COLOR_PINK);
    modal->getRenderer()->setBorders(2);
    return modal;
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
    button->onClick(callback);

    return button;
}

void AppStateWeaponModification::onResume()
{
    auto& inv = scene.actors.get<WeaponInventory>(scene.playerEntity);

    inv.weapons[0] = WeaponBuilder::createWeapon(
        EntityKind::Player,
        {
            scene.loadout.weapon1Modules[0],
            scene.loadout.weapon1Modules[1],
            scene.loadout.weapon1Modules[2],
        });
    inv.weapons[1] = WeaponBuilder::createWeapon(
        EntityKind::Player,
        {
            scene.loadout.weapon2Modules[0],
            scene.loadout.weapon2Modules[1],
            scene.loadout.weapon2Modules[2],
        });

    app.popState(Messaging::serialize<PopIfNotGame>());
}

void AppStateWeaponModification::onBack()
{
    app.popState();
}

void AppStateWeaponModification::onCycle()
{
    currentWeaponIdx = (currentWeaponIdx + 1) % 2;
    animationTimer.restart();
}

void AppStateWeaponModification::onModSelected(size_t moduleIdx)
{
    auto&& modal =
        createModuleSelectModal(StringId::SelectModule, { "80%", "80%" });

    auto&& close = [&, modal]
    {
        auto&& widget = dic.gui.get<tgui::Panel>("ModalContainer");
        dic.gui.remove(widget);
        modal->close();
    };

    auto&& background = tgui::Panel::create();
    background->getRenderer()->setBackgroundColor({ 0, 0, 0, 128 });
    background->onClick(close);
    dic.gui.add(background, "ModalContainer");
    dic.gui.add(modal);

    auto&& content = tgui::ScrollablePanel::create();
    modal->add(content);

    const int MODULES_PER_ROW = 8;
    const float BUTTON_SIZE = 100 / MODULES_PER_ROW;
    int x = 0;
    int y = 0;
    for (auto&& module : getAvailableModules())
    {
        const bool isInUse =
            module != WeaponModule::None
            && (uni::ranges::contains(scene.loadout.weapon1Modules, module)
                || uni::ranges::contains(scene.loadout.weapon2Modules, module));

        auto cellLayout = tgui::Group::create(
            { uni::format("{}%", BUTTON_SIZE).c_str(), "width" });
        cellLayout->setPosition({
            uni::format("width * {}", x).c_str(),
            uni::format("height * {}", y).c_str(),
        });

        cellLayout->add(createModuleSelectButton(
            module,
            [&, close, module, moduleIdx]
            {
                dic.soundPlayer.playClick();
                getCurrentLoadout()[moduleIdx] = module;
                close();
            },
            isInUse));

        content->add(cellLayout);

        x++;
        if (x == MODULES_PER_ROW)
        {
            x = 0;
            y++;
        }
    }
}

namespace
{
    constexpr std::array<WeaponModule, 10> ALL_MODULES = {
        WeaponModule::None,
        WeaponModule::SpreadBarrel_x2,
        WeaponModule::SpreadBarrel_x4,
        WeaponModule::CadenceBarrel,
        WeaponModule::ExplosiveAmmo,
        WeaponModule::Ricochet,
        WeaponModule::PassthruAmmo,
        WeaponModule::BigBullet,
        WeaponModule::Spikes,
        WeaponModule::Splitter,
    };

    constexpr std::array<StringId, 10> MODULE_STRING_IDS = {
        StringId::WeaponModule_None,
        StringId::WeaponModule_SpreadBarrelx2,
        StringId::WeaponModule_SpreadBarrel,
        StringId::WeaponModule_CadenceBarrel,
        StringId::WeaponModule_ExplosiveAmmo,
        StringId::WeaponModule_Ricochet,
        StringId::WeaponModule_PassthruAmmo,
        StringId::WeaponModule_BigBullet,
        StringId::WeaponModule_Spikes,
        StringId::WeaponModule_Splitter,
    };

    constexpr std::array<StringId, 3> SLOT_LABEL_IDS = {
        StringId::WeaponModuleSlot1,
        StringId::WeaponModuleSlot2,
        StringId::WeaponModuleSlot3,
    };

} // namespace

// ---- AppStateWeaponModification ---------------------------------------------

std::vector<std::string> AppStateWeaponModification::getModuleNames() const
{
    std::vector<std::string> names;
    names.reserve(MODULE_STRING_IDS.size());
    for (auto id : MODULE_STRING_IDS)
        names.push_back(dic.strings.getString(id));
    return names;
}

std::vector<std::string>
AppStateWeaponModification::getAvailableModuleNames() const
{
    std::vector<std::string> names;
    // None is always available
    names.push_back(dic.strings.getString(StringId::WeaponModule_None));
    for (size_t i = 1; i < ALL_MODULES.size(); ++i)
    {
        if (scene.loadout.unlockedModules.contains(ALL_MODULES[i]))
            names.push_back(dic.strings.getString(MODULE_STRING_IDS[i]));
    }
    return names;
}

// Build the subset of available modules (None + unlocked)
std::vector<WeaponModule>
AppStateWeaponModification::getAvailableModules() const
{
    std::vector<WeaponModule> modules;
    modules.push_back(WeaponModule::None);
    for (size_t i = 1; i < ALL_MODULES.size(); ++i)
    {
        if (scene.loadout.unlockedModules.contains(ALL_MODULES[i]))
            modules.push_back(ALL_MODULES[i]);
    }
    return modules;
}

size_t AppStateWeaponModification::moduleToIndex(WeaponModule module) noexcept
{
    for (size_t i = 0; i < ALL_MODULES.size(); ++i)
        if (ALL_MODULES[i] == module) return i;
    return 0; // fallback: None
}

WeaponModule AppStateWeaponModification::indexToModule(size_t index) noexcept
{
    if (index < ALL_MODULES.size()) return ALL_MODULES[index];
    return WeaponModule::None;
}
