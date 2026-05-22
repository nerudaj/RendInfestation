#include "appstate/AppStateWeaponModification.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "game/builders/WeaponBuilder.hpp"
#include "game/definitions/Components.hpp"
#include "gui/builders/FormBuilder.hpp"
#include "gui/builders/WidgetBuilder.hpp"
#include "strings/StringId.hpp"
#include <array>

enum class [[nodiscard]] WorkbenchSpriteId
{
    Ui,
    BigMag,
    BigNozzle,
    LongBarrel,
    BaseGun,
    Table,
};

// ---- module <-> index helpers -----------------------------------------------

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

AppStateWeaponModification::AppStateWeaponModification(
    dgm::App& app, DependencyContainer& dic, GameScene& scene)
    : dgm::AppState(app)
    , dic(dic)
    , scene(scene)
    , renderCamera(
          sf::FloatRect { sf::Vector2f { 0.f, 0.f },
                          sf::Vector2f { 1.f, 1.f } },
          INTERNAL_GAME_RESOLUTION)
    , guiCamera(
          sf::FloatRect { sf::Vector2f { 0.f, 0.f },
                          sf::Vector2f { 1.f, 1.f } },
          sf::Vector2f(app.window.getSize()))
    , workbenchTexture(dic.resmgr.get<sf::Texture>("workbench.png"))
    , workbenchClip(dic.resmgr.get<dgm::Clip>("workbench.png.clip"))
    , workbenchSprite(workbenchTexture)
    , moduleIconTexture(dic.resmgr.get<sf::Texture>("infestation_modules.png"))
    , moduleIconClip(dic.resmgr.get<dgm::Clip>("infestation_modules.png.clip"))
    , moduleIconSprite(moduleIconTexture)
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
    app.window.setViewFromCamera(renderCamera);
    renderWorkbench();

    app.window.setViewFromCamera(guiCamera);

    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateWeaponModification::renderWorkbench()
{
    workbenchSprite.setTextureRect(
        workbenchClip.getFrame(WorkbenchSpriteId::Table));
    app.window.draw(workbenchSprite);

    workbenchSprite.setTextureRect(
        workbenchClip.getFrame(WorkbenchSpriteId::BaseGun));
    app.window.draw(workbenchSprite);

    if (uni::ranges::contains(getCurrentLoadout(), WeaponModule::BigBullet))
    {
        workbenchSprite.setTextureRect(
            workbenchClip.getFrame(WorkbenchSpriteId::BigMag));
        app.window.draw(workbenchSprite);
    }

    if (uni::ranges::contains(getCurrentLoadout(), WeaponModule::CadenceBarrel))
    {
        workbenchSprite.setTextureRect(
            workbenchClip.getFrame(WorkbenchSpriteId::LongBarrel));
        app.window.draw(workbenchSprite);
    }

    if (uni::ranges::contains(
            getCurrentLoadout(), WeaponModule::SpreadBarrel_x2)
        || uni::ranges::contains(
            getCurrentLoadout(), WeaponModule::SpreadBarrel_x4))
    {
        workbenchSprite.setTextureRect(
            workbenchClip.getFrame(WorkbenchSpriteId::BigNozzle));
        app.window.draw(workbenchSprite);
    }

    workbenchSprite.setTextureRect(
        workbenchClip.getFrame(WorkbenchSpriteId::Ui));
    app.window.draw(workbenchSprite);

    for (auto&& [idx, module] : uni::views::enumerate(getCurrentLoadout()))
    {
        if (module == WeaponModule::None) continue;
        moduleIconSprite.setTextureRect(moduleIconClip.getFrame(module));
        moduleIconSprite.setPosition({ 36.f + idx * 23.f, 164.f });
        app.window.draw(moduleIconSprite);
    }
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

    auto&& createButton = [](auto&& callback)
    {
        auto&& button = tgui::Button::create();
        button->getRenderer()->setBackgroundColor(sf::Color::Transparent);
        button->getRenderer()->setBorders(0u);
        button->setSize({ "100%", "100%" });
        button->onClick(std::forward<decltype(callback)>(callback));
        return button;
    };

    auto&& resumeButtonLayout = tgui::Group::create();
    resumeButtonLayout->setSize(toLayout(24, 24));
    resumeButtonLayout->setPosition(toLayout(346, 178));
    resumeButtonLayout->add(createButton([&] { onResume(); }));
    dic.gui.add(resumeButtonLayout);

    auto&& cancelButtonLayout = tgui::Group::create();
    cancelButtonLayout->setSize(toLayout(24, 24));
    cancelButtonLayout->setPosition(toLayout(16, 16));
    cancelButtonLayout->add(createButton([&] { onBack(); }));
    dic.gui.add(cancelButtonLayout);

    auto&& cycleLeftLayout = tgui::Group::create();
    cycleLeftLayout->setSize(toLayout(19, 47));
    cycleLeftLayout->setPosition(toLayout(17, 85));
    cycleLeftLayout->add(createButton([&] { onCycle(); }));
    dic.gui.add(cycleLeftLayout);

    auto&& cycleRightLayout = tgui::Group::create();
    cycleRightLayout->setSize(toLayout(19, 47));
    cycleRightLayout->setPosition(toLayout(348, 85));
    cycleRightLayout->add(createButton([&] { onCycle(); }));
    dic.gui.add(cycleRightLayout);

    auto&& modSelect1Layout = tgui::Group::create();
    modSelect1Layout->setSize(toLayout(18, 18));
    modSelect1Layout->setPosition(toLayout(35, 163));
    modSelect1Layout->add(createButton([&] { onModSelected(0); }));
    dic.gui.add(modSelect1Layout);

    auto&& modSelect2Layout = tgui::Group::create();
    modSelect2Layout->setSize(toLayout(18, 18));
    modSelect2Layout->setPosition(toLayout(58, 163));
    modSelect2Layout->add(createButton([&] { onModSelected(1); }));
    dic.gui.add(modSelect2Layout);

    auto&& modSelect3Layout = tgui::Group::create();
    modSelect3Layout->setSize(toLayout(18, 18));
    modSelect3Layout->setPosition(toLayout(81, 163));
    modSelect3Layout->add(createButton([&] { onModSelected(2); }));
    dic.gui.add(modSelect3Layout);
}

void AppStateWeaponModification::_buildLayout()
{
    const auto availableModules = getAvailableModules();
    const auto availableModuleNames = getAvailableModuleNames();

    // Helper: returns the index within availableModules for a given module.
    // Falls back to 0 (None) if the module is not available.
    auto toAvailableIndex = [&](WeaponModule module) -> size_t
    {
        for (size_t i = 0; i < availableModules.size(); ++i)
            if (availableModules[i] == module) return i;
        return 0;
    };

    // Helper: build one weapon column as a FormBuilder panel
    auto buildWeaponColumn =
        [&](StringId titleId,
            std::array<WeaponModule, 3>& modules) -> tgui::Container::Ptr
    {
        auto builder = FormBuilder(dic.strings, dic.sizer);

        for (size_t slot = 0; slot < 3; ++slot)
        {
            const size_t capturedSlot = slot;

            // If the currently selected module is no longer available, reset
            // it to None
            if (!scene.loadout.unlockedModules.contains(modules[capturedSlot])
                && modules[capturedSlot] != WeaponModule::None)
            {
                modules[capturedSlot] = WeaponModule::None;
            }

            auto dropdown = WidgetBuilder::createDropdown(
                availableModuleNames,
                availableModuleNames[toAvailableIndex(modules[capturedSlot])],
                [&modules, capturedSlot, availableModules](size_t idx)
                {
                    if (idx < availableModules.size())
                        modules[capturedSlot] = availableModules[idx];
                },
                dic.sizer);

            builder.addOption(SLOT_LABEL_IDS[slot], dropdown);
        }

        auto panel = tgui::Group::create();

        auto heading = WidgetBuilder::createHeading(
            dic.strings.getString(titleId), dic.sizer, HeadingLevel::H2);
        heading->setSize({ "100%",
                           tgui::String(std::to_string(static_cast<int>(
                               dic.sizer.getBaseContainerHeight()))) });

        auto form = builder.build();
        form->setPosition(
            { 0.f, static_cast<float>(dic.sizer.getBaseContainerHeight()) });
        form->setSize({ "100%", "100%" });

        panel->add(heading);
        panel->add(form);

        return panel;
    };

    // Two-column horizontal layout
    auto twoColumns = tgui::HorizontalLayout::create();
    twoColumns->add(buildWeaponColumn(
        StringId::Weapon1Title, scene.loadout.weapon1Modules));
    twoColumns->addSpace(0.05f);
    twoColumns->add(buildWeaponColumn(
        StringId::Weapon2Title, scene.loadout.weapon2Modules));

    dic.gui.rebuildWith(
        dic.guiBuilderFactory.createDefaultLayoutBuilder()
            .withNoBackground()
            .withTitle(StringId::WeaponModification, HeadingLevel::H1)
            .withContent(twoColumns)
            .withNoTopLeftButton()
            .withNoTopRightButton()
            .withNoBottomLeftButton()
            .withBottomRightButton(StringId::Resume, [&] { onResume(); })
            .build());
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
    // TODO: trigger move animation
}

void AppStateWeaponModification::onModSelected(size_t moduleIdx)
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

    auto&& modal = tgui::ChildWindow::create(
        dic.strings.getString(StringId::WeaponModification));
    modal->setSize({ "80%", "80%" });
    modal->setPosition({ "10%", "10%" });
    modal->setCloseBehavior(tgui::ChildWindow::CloseBehavior::Remove);
    modal->getRenderer()->setTitleBarColor(COLOR_PURPLE);
    modal->getRenderer()->setTitleBarHeight(dic.sizer.getBaseContainerHeight());
    modal->getRenderer()->setTextSize(dic.sizer.getBaseFontSize());
    modal->getRenderer()->setBorderColor(COLOR_PINK);
    modal->getRenderer()->setBorders(2);
    dic.gui.add(modal);

    auto&& content = tgui::ScrollablePanel::create();
    modal->add(content);

    int x = 0;
    int y = 0;
    for (auto&& module : getAvailableModules())
    {
        auto cellLayout = tgui::Group::create({ "12.5%", "width" });
        cellLayout->setPosition({
            uni::format("width * {}", x).c_str(),
            uni::format("height * {}", y).c_str(),
        });

        auto&& button =
            tgui::Button::create(module == WeaponModule::None ? "X" : "");

        if (module != WeaponModule::None)
        {
            button->getRenderer()->setTexture(dic.resmgr.get<tgui::Texture>(
                uni::format("ModuleIcon-{}", std::to_underlying(module))));
        }
        button->setSize(toLayout(18, 18));
        button->setPosition({ "parent.width / 2 - width / 2",
                              "parent.height / 2 - height  / 2" });
        button->onClick(
            [&, module, moduleIdx, modal]
            {
                getCurrentLoadout()[moduleIdx] = module;
                modal->close();
            });
        cellLayout->add(button);

        content->add(cellLayout);

        x++;
        if (x == 8)
        {
            x = 0;
            y++;
        }
    }
}
