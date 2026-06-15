#include "appstate/AppStateChooseBonus.hpp"
#include "appstate/CommonHandler.hpp"
#include <misc/Compatibility.hpp>
#include <random>

void AppStateChooseBonus::input()
{
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

tgui::Layout2d AppStateChooseBonus::getCoreLayoutSize()
{
#ifdef ANDROID
    { "60%", "50%" };
#else
    return { "40%", "40%" };
#endif
}

void AppStateChooseBonus::buildLayout()
{
    auto content = tgui::Group::create(getCoreLayoutSize());
    content->setPosition(
        { "parent.width / 2 - width / 2", "parent.height / 2 - height / 2" });
    auto panel = tgui::Panel::create();

    auto innerLayout = tgui::Group::create({ "100%", "10%" });
    innerLayout->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(StringId::ChooseBonus),
        dic.sizer,
        "justify"_true));
    panel->add(innerLayout);

    auto modulesToPick = generatePickerSelection();

    auto leftBox = createSelectablePanel({ "50%", "90%" });
    leftBox->setPosition({ "0%", "10%" });
    leftBox->add(createPickerBox(modulesToPick.first));
    leftBox->onClick([&, module = modulesToPick.first] { onSubmit(module); });
    panel->add(leftBox);

    auto rightBox = createSelectablePanel({ "50%", "90%" });
    rightBox->setPosition({ "50%", "10%" });
    rightBox->add(createPickerBox(modulesToPick.second));
    rightBox->onClick([&, module = modulesToPick.second] { onSubmit(module); });
    panel->add(rightBox);
    content->add(panel);

    dic.gui.rebuildWith(content);
}

tgui::Panel::Ptr AppStateChooseBonus::createSelectablePanel(tgui::Layout2d size)
{
    auto panel = tgui::Panel::create(size);
    panel->getRenderer()->setBorders(0);
    panel->onMouseEnter(
        [=] { panel->getRenderer()->setBackgroundColor(COLOR_LIGHT_GREY); });
    panel->onMouseLeave(
        [=] { panel->getRenderer()->setBackgroundColor(COLOR_DARK_GREY); });
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

    auto descriptionLayout = tgui::Group::create({ "90%", "55%" });
    descriptionLayout->setPosition({ "5%", "40%" });
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

    if (scene.unlockedModules.size() <= 3)
    {
        draftPool.insert(WeaponModule::SpreadBarrel_x2);
        draftPool.insert(WeaponModule::BigBullet);
        draftPool.insert(WeaponModule::Spikes);
        draftPool.insert(WeaponModule::Ricochet);
        draftPool.insert(WeaponModule::PassthruAmmo);
        draftPool.insert(WeaponModule::Push);
    }
    else if (scene.unlockedModules.size() <= 6)
    {
        draftPool.insert(WeaponModule::CadenceBarrel);
        draftPool.insert(WeaponModule::SpreadBarrel_x4);
        draftPool.insert(WeaponModule::Splitter);
    }
    else
    {
        draftPool.insert(WeaponModule::ExplosiveAmmo);
        draftPool.insert(WeaponModule::Turret);
    }

    if (scene.unlockedModules.contains(WeaponModule::SpreadBarrel_x2))
        draftPool.insert(WeaponModule::SpreadBarrel_x4);
    else if (scene.unlockedModules.contains(WeaponModule::Spikes))
        draftPool.insert(WeaponModule::ExplosiveAmmo);

    for (auto&& unlocked : scene.unlockedModules)
    {
        if (draftPool.contains(unlocked))
        {
            draftPool.erase(unlocked);
        }
    }

    // TODO: There has to be something to always return on higher ranks

    auto sortablePool = draftPool | uni::ranges::to<std::vector>();
    std::mt19937 gen { static_cast<unsigned>(rand()) };
    uni::ranges::shuffle(sortablePool, gen);

    assert(sortablePool.size() >= 2);

    return { sortablePool[0], sortablePool[1] };
}

void AppStateChooseBonus::onSubmit(WeaponModule module)
{
    scene.unlockedModules.insert(module);
    app.popState();
}
