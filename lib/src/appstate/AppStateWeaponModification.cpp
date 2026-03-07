#include "appstate/AppStateWeaponModification.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "game/builders/WeaponBuilder.hpp"
#include "game/definitions/Actor.hpp"
#include "gui/builders/FormBuilder.hpp"
#include "gui/builders/WidgetBuilder.hpp"
#include "strings/StringId.hpp"
#include <array>

// ---- module <-> index helpers -----------------------------------------------

namespace
{

    constexpr std::array<WeaponModule, 8> ALL_MODULES = {
        WeaponModule::None,          WeaponModule::SpreadBarrel,
        WeaponModule::CadenceBarrel, WeaponModule::ExplosiveAmmo,
        WeaponModule::Ricochet,      WeaponModule::PassthruAmmo,
        WeaponModule::BigBullet,     WeaponModule::Spikes,
    };

    constexpr std::array<StringId, 8> MODULE_STRING_IDS = {
        StringId::WeaponModule_None,
        StringId::WeaponModule_SpreadBarrel,
        StringId::WeaponModule_CadenceBarrel,
        StringId::WeaponModule_ExplosiveAmmo,
        StringId::WeaponModule_Ricochet,
        StringId::WeaponModule_PassthruAmmo,
        StringId::WeaponModule_BigBullet,
        StringId::WeaponModule_Spikes,
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
    const auto moduleNames = getModuleNames();

    // Helper: build one weapon column as a FormBuilder panel
    auto buildWeaponColumn =
        [&](StringId titleId,
            std::array<WeaponModule, 3>& modules) -> tgui::Container::Ptr
    {
        auto builder = FormBuilder(dic.strings, dic.sizer);

        for (size_t slot = 0; slot < 3; ++slot)
        {
            // Capture slot index and modules array by value/ref appropriately
            const size_t capturedSlot = slot;
            auto dropdown = WidgetBuilder::createDropdown(
                moduleNames,
                dic.strings.getString(
                    MODULE_STRING_IDS[moduleToIndex(modules[capturedSlot])]),
                [&modules, capturedSlot](size_t idx)
                { modules[capturedSlot] = indexToModule(idx); },
                dic.sizer);

            builder.addOption(SLOT_LABEL_IDS[slot], dropdown);
        }

        auto panel = tgui::Panel::create();

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
        dic.guiBuilderFactory.createDefaultLayoutBuiler()
            .withNoBackgroundImage()
            .withTitle(
                dic.strings.getString(StringId::WeaponModification),
                HeadingLevel::H1)
            .withContent(twoColumns)
            .withNoTopLeftButton()
            .withNoTopRightButton()
            .withNoBottomLeftButton()
            .withBottomRightButton(WidgetBuilder::createButton(
                dic.strings.getString(StringId::Resume),
                [&] { onResume(); },
                dic.sizer,
                dic.soundPlayer))
            .build());
}

void AppStateWeaponModification::onResume()
{
    auto& inv = scene.actors.get<WeaponInventory>(scene.playerEntity);

    inv.weapons[0] = WeaponBuilder::createWeapon({
        scene.loadout.weapon1Modules[0],
        scene.loadout.weapon1Modules[1],
        scene.loadout.weapon1Modules[2],
    });
    inv.weapons[1] = WeaponBuilder::createWeapon({
        scene.loadout.weapon2Modules[0],
        scene.loadout.weapon2Modules[1],
        scene.loadout.weapon2Modules[2],
    });

    app.popState(Messaging::serialize<PopIfNotGame>());
}
