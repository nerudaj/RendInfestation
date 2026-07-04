#include "appstate/AppStateChooseBonus.hpp"
#include "appstate/AppStatePause.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "gui/GuiBuilderHelper.hpp"
#include <misc/Compatibility.hpp>
#include <random>

void AppStateChooseBonus::input()
{
    if (dic.input.isPauseButtonPressed())
    {
        app.pushState<AppStatePause>(dic, scene);
        // If pause button is the same as back button, we need
        // to release it to prevent pause menu from immediately closing again
        dic.input.forceRelease(InputKind::BackButton);
    }

    CommonHandler::handleInput(
        app,
        dic,
        dic.settings.input,
        {
            .disableGoBack = true,
        });
}

void AppStateChooseBonus::update() {}

void AppStateChooseBonus::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateChooseBonus::restoreFocusImpl(const std::string& msg)
{
    if (auto message = Messaging::deserialize(msg))
    {
        std::visit(
            overloads {
                [&](PopIfNotMenu&) { app.popState(msg); },
                [&](PopIfNotGame&) {},
            },
            *message);
    }

    buildLayout();
}

tgui::Layout2d AppStateChooseBonus::getCoreLayoutSize()
{
#ifdef ANDROID
    return { "60%", "50%" };
#else
    return { "60%", "50%" };
#endif
}

void AppStateChooseBonus::buildLayout()
{
    auto content = tgui::Group::create(getCoreLayoutSize());
    GuiBuilderHelper::centerWidget(content);

    auto headingPanel = tgui::Panel::create({ "100%", "10%" });
    content->add(headingPanel);
    headingPanel->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(StringId::ChooseBonus),
        dic.sizer,
        "justify"_true));

    auto modulesToPick = generatePickerSelection();

    auto leftBox = createSelectablePanel({ "49%", "85%" });
    leftBox->setPosition({ "0%", "15%" });
    leftBox->add(createPickerBox(modulesToPick.first));
    leftBox->onClick([&, module = modulesToPick.first] { onSubmit(module); });
    content->add(leftBox);

    auto rightBox = createSelectablePanel({ "49%", "85%" });
    rightBox->setPosition({ "51%", "15%" });
    rightBox->add(createPickerBox(modulesToPick.second));
    rightBox->onClick([&, module = modulesToPick.second] { onSubmit(module); });
    content->add(rightBox);

    dic.gui.rebuildWith(content);
}

tgui::Panel::Ptr AppStateChooseBonus::createSelectablePanel(tgui::Layout2d size)
{
    auto panel = tgui::Panel::create(size);
    panel->setRenderer(
        tgui::Theme::getDefault()->getRenderer("SelectablePanel"));
    panel->onMouseEnter(
        [panel = panel, this]
        {
            panel->setRenderer(
                tgui::Theme::getDefault()->getRenderer("SelectablePanelHover"));
        });
    panel->onMouseLeave(
        [panel = panel, this]
        {
            panel->setRenderer(
                tgui::Theme::getDefault()->getRenderer("SelectablePanel"));
        });
    return panel;
}

tgui::Container::Ptr AppStateChooseBonus::createPickerBox(WeaponModule module)
{
    auto content = tgui::Group::create();

    auto imageLayout = tgui::Group::create({ "100%", "30%" });
    imageLayout->setPosition({ "0%", "5%" });
    content->add(imageLayout);

    auto image = tgui::Panel::create({ "height", "100%" });
    image->setPosition({ "parent.width / 2 - width / 2", "0%" });
    image->getRenderer()->setTextureBackground(dic.resmgr.get<tgui::Texture>(
        uni::format("ModuleIcon-{}", std::to_underlying(module))));
    imageLayout->add(image);

    auto descriptionLayout = tgui::Group::create({ "90%", "45%" });
    descriptionLayout->setPosition({ "5%", "50%" });
    content->add(descriptionLayout);

    auto text = WidgetBuilder::createTextLabel(
        dic.strings.getString(getModuleDescription(module)), dic.sizer);
    text->setVerticalAlignment(tgui::VerticalAlignment::Top);
    descriptionLayout->add(text);

    return content;
}

std::pair<WeaponModule, WeaponModule>
AppStateChooseBonus::generatePickerSelection()
{
    std::set<WeaponModule> draftPool;

    // Base unlock progression
    draftPool.insert(WeaponModule::SpreadBarrel_x2);
    draftPool.insert(WeaponModule::BigBullet);
    draftPool.insert(WeaponModule::Spikes);
    draftPool.insert(WeaponModule::Ricochet);
    draftPool.insert(WeaponModule::PassthruAmmo);
    draftPool.insert(WeaponModule::Push);

    if (scene.unlockedModules.size() >= 3)
    {
        draftPool.insert(WeaponModule::CadenceBarrel);
        draftPool.insert(WeaponModule::SpreadBarrel_x4);
        draftPool.insert(WeaponModule::Splitter);
    }

    if (scene.unlockedModules.size() >= 6)
    {
        draftPool.insert(WeaponModule::ExplosiveAmmo);
        draftPool.insert(WeaponModule::Turret);
    }

    // Out-of-line progression
    if (scene.unlockedModules.contains(WeaponModule::SpreadBarrel_x2))
        draftPool.insert(WeaponModule::SpreadBarrel_x4);

    if (scene.unlockedModules.contains(WeaponModule::Spikes)
        || scene.unlockedModules.contains(WeaponModule::BigBullet))
    {
        draftPool.insert(WeaponModule::ExplosiveAmmo);
    }

    if (scene.unlockedModules.contains(WeaponModule::CadenceBarrel))
    {
        draftPool.insert(WeaponModule::Turret);
    }

    if (scene.unlockedModules.size() >= 3 && scene.loadout.weapons.size() == 1)
    {
        draftPool.insert(WeaponModule::ExtraGun);
    }

    // Clear already unlocked modules
    for (auto&& unlocked : scene.unlockedModules)
    {
        if (draftPool.contains(unlocked))
        {
            draftPool.erase(unlocked);
        }
    }

    auto sortablePool = draftPool | uni::ranges::to<std::vector>();
    std::mt19937 gen { static_cast<unsigned>(rand()) };
    uni::ranges::shuffle(sortablePool, gen);

    while (sortablePool.size() < 2)
    {
        sortablePool.push_back(WeaponModule::ExtraHealth);
    }

    if (scene.actors.get<Health>(scene.playerEntity).value
        < scene.playerMaxHealth / 2)
    {
        sortablePool[1] = WeaponModule::ExtraHealth;
    }

    assert(sortablePool.size() >= 2);
    return { sortablePool[0], sortablePool[1] };
}

void AppStateChooseBonus::onSubmit(WeaponModule module)
{
    if (module == WeaponModule::ExtraGun)
    {
        scene.loadout.weapons.push_back(WeaponConfig {});
    }
    else if (module == WeaponModule::ExtraHealth)
    {
        scene.playerMaxHealth += 10;
        scene.actors.get<Health>(scene.playerEntity) =
            Health(scene.playerMaxHealth);
    }
    else
    {
        scene.unlockedModules.insert(module);
        if (scene.unlockedModules.size() <= 3)
        {
            for (auto& modInWeapon : scene.loadout.weapons.front().modules)
            {
                if (modInWeapon == WeaponModule::None)
                {
                    modInWeapon = module;
                    break;
                }
            }
        }
    }

    scene.updatePlayerLoadout();
    app.popState();
}
