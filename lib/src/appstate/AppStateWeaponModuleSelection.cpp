#include "appstate/AppStateWeaponModuleSelection.hpp"
#include "appstate/CommonHandler.hpp"
#include "gui/GuiBuilderHelper.hpp"

AppStateWeaponModuleSelection::AppStateWeaponModuleSelection(
    dgm::App& app,
    DependencyContainer& dic,
    const GameScene& scene,
    std::optional<WeaponModule>& outModule,
    const WeaponModule currentlySelectedModule)
    : dgm::AppState(
          app,
          {
              .shouldDrawUnderlyingState = true,
          })
    , dic(dic)
    , scene(scene)
    , outModule(outModule)
    , selectedModule(currentlySelectedModule)
    , moduleDetailComponent(
          dic,
          [&](const WeaponModule module)
          { return getWeaponModuleTexture(module); })
{
    buildLayout();
}

AppStateWeaponModuleSelection::~AppStateWeaponModuleSelection()
{
    auto&& modal = dic.gui.get<tgui::ChildWindow>("Modal");
    modal->close();
    dic.gui.remove(modal);
    dic.gui.remove(dic.gui.get<tgui::Panel>("ModalBackground"));
}

void AppStateWeaponModuleSelection::input()
{
    CommonHandler::handleInput(
        app, dic, dic.settings.input, CommonHandlerOptions {});
}

void AppStateWeaponModuleSelection::update() {}

void AppStateWeaponModuleSelection::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

tgui::ChildWindow::Ptr AppStateWeaponModuleSelection::createModuleSelectModal(
    StringId titleStringId, tgui::Layout2d size) const
{
    auto&& modal = tgui::ChildWindow::create(
        dic.strings.getString(titleStringId),
        tgui::ChildWindow::TitleButton::None);
    modal->setSize(size);
    modal->setTitleTextSize(dic.sizer.getBaseFontSize());
    TguiHelper::centerInParent(modal);
    modal->getRenderer()->setTitleBarColor(COLOR_MID_BLUE);
    modal->getRenderer()->setTitleBarHeight(
        static_cast<float>(dic.sizer.getBaseContainerHeight()));
    modal->getRenderer()->setTextSize(dic.sizer.getBaseFontSize());
    return modal;
}

void AppStateWeaponModuleSelection::buildLayout()
{
    auto&& modal =
        createModuleSelectModal(StringId::SelectModule, { "80%", "80%" });
    modal->add(buildModuleSelectionLayout());

    auto&& background = GuiBuilderHelper::createSemitransparentBlackPanel();
    dic.gui.add(background, "ModalBackground");
    background->onClick([] {});
    dic.gui.add(modal, "Modal");
}

tgui::Container::Ptr AppStateWeaponModuleSelection::buildModuleSelectionLayout()
{
    auto&& container = tgui::Group::create();

    auto&& hbox = tgui::HorizontalLayout::create(
        { "100%", "100% - ModuleSelectionBottomNavbar.height" });
    container->add(hbox);

    auto&& bottomNavbar = WidgetBuilder::createContainer<tgui::Panel>(
        { "100%", dic.sizer.getBaseContainerHeight() * 1.5 },
        {
            .className = "BottomCutoutPanel",
        });
    TguiHelper::alignInParent(
        bottomNavbar,
        tgui::HorizontalAlignment::Left,
        tgui::VerticalAlignment::Bottom);
    container->add(bottomNavbar, "ModuleSelectionBottomNavbar");

    auto&& createSubmitButton =
        [&](StringId stringId,
            std::function<void(void)>&& callback,
            const std::optional<std::string> id = std::nullopt,
            bool enabled = true)
    {
        auto&& wrapper = tgui::Group::create(
            { dic.sizer.getBaseContainerHeight() * 4, "90%" });
        wrapper->add(WidgetBuilder::createRowButton(
            dic.strings.getString(stringId),
            std::forward<decltype(callback)>(callback),
            dic.sizer,
            dic.soundPlayer,
            {
                .id = id,
                .enabled = enabled,
            }));
        return wrapper;
    };

    auto&& buttonList = tgui::GrowHorizontalLayout::create();
    buttonList->add(createSubmitButton(StringId::Back, [&] { onCancel(); }));
    buttonList->add(createSubmitButton(
        StringId::RemoveMod,
        [&] { onRemoveMod(); },
        std::nullopt,
        selectedModule != WeaponModule::None));
    buttonList->add(createSubmitButton(
        StringId::Install, [&] { onApply(); }, "ApplyButton"));
    TguiHelper::alignInParent(
        buttonList,
        tgui::HorizontalAlignment::Right,
        tgui::VerticalAlignment::Center);
    bottomNavbar->add(buttonList);

    auto&& scrollList = WidgetBuilder::createContainer<tgui::ScrollablePanel>({
        .className = "AllCornersPanelScrollable",
    });
    scrollList->getRenderer()->setPadding({ 20.f, 20.f });
    hbox->add(scrollList);

    auto&& moduleList = tgui::GrowVerticalLayout::create();
    scrollList->add(moduleList);

    for (auto&& module : scene.unlockedModules)
    {
        if (module == WeaponModule::None) continue;
        moduleList->add(
            buildModuleListEntryLayout(module), getModuleListEntryId(module));
    }

    hbox->add(moduleDetailComponent.buildLayout());

    return container;
}

tgui::Container::Ptr AppStateWeaponModuleSelection::buildModuleListEntryLayout(
    const WeaponModule module)
{
    auto&& row = WidgetBuilder::createContainer<tgui::Panel>(
        { "100%", dic.sizer.getBaseContainerHeight() },
        {
            .className = "UntexturedWidget",
        });

    const bool isInUse = isInUseModule(module);
    if (isInUse) row->getRenderer()->setOpacity(0.5f);

    if (module == selectedModule)
    {
        row->getRenderer()->setBackgroundColor(COLOR_MID_BLUE);
    }

    row->onMouseEnter(
        [this, rowCopy = row, moduleCopy = module]
        {
            if (moduleCopy != selectedModule)
                rowCopy->getRenderer()->setBackgroundColor(COLOR_LIGHT_BLUE);
        });
    row->onMouseLeave(
        [this, rowCopy = row, moduleCopy = module]
        {
            if (moduleCopy != selectedModule)
                rowCopy->getRenderer()->setBackgroundColor(
                    tgui::Color::Transparent);
        });
    row->onClick([this, moduleCopy = module] { onModuleSelected(moduleCopy); });

    const auto&& iconId =
        std::format("ModuleIcon{}", std::to_underlying(module));

    row->add(
        [&]
        {
            auto&& iconGroup = tgui::Group::create({ "height", "100%" });

            auto&& panel = tgui::Panel::create({ "90%", "90%" });
            TguiHelper::centerInParent(panel);
            panel->getRenderer()->setTextureBackground(
                getWeaponModuleTexture(module));
            iconGroup->add(panel);

            return iconGroup;
        }(),
        iconId);

    row->add(
        [&]
        {
            auto&& labelGroup = tgui::Group::create(
                { std::format("100% - {}.width", iconId).c_str(), "100%" });

            TguiHelper::alignInParent(
                labelGroup,
                tgui::HorizontalAlignment::Right,
                tgui::VerticalAlignment::Top);

            auto&& labelText = std::format(
                "{} {}",
                dic.strings.getString(getModuleName(module)),
                isInUse ? dic.strings.getString(StringId::InUse) : "");

            labelGroup->add(WidgetBuilder::createTextLabel(
                labelText, dic.sizer, "justify"_true));

            return labelGroup;
        }());

    return row;
}

void AppStateWeaponModuleSelection::onCancel()
{
    outModule = std::nullopt;
    app.popState();
}

void AppStateWeaponModuleSelection::onApply()
{
    dic.soundPlayer.playPovSound(SoundId::UiModInstall);
    outModule = selectedModule;
    app.popState();
}

void AppStateWeaponModuleSelection::onRemoveMod()
{
    dic.soundPlayer.playPovSound(SoundId::UiModUninstall);
    outModule = WeaponModule::None;
    app.popState();
}

void AppStateWeaponModuleSelection::onModuleSelected(
    const WeaponModule newSelectedModule)
{
    if (selectedModule != WeaponModule::None)
    {
        dic.gui.get<tgui::Panel>(getModuleListEntryId(selectedModule))
            ->getRenderer()
            ->setBackgroundColor(tgui::Color::Transparent);
    }

    dic.gui.get<tgui::Panel>(getModuleListEntryId(newSelectedModule))
        ->getRenderer()
        ->setBackgroundColor(COLOR_MID_BLUE);

    selectedModule = newSelectedModule;
    moduleDetailComponent.refreshDetail(selectedModule);

    auto&& enabled = !isInUseModule(newSelectedModule);
    auto&& button = dic.gui.get<tgui::Button>("ApplyButton");
    button->setEnabled(enabled);
    button->getRenderer()->setOpacity(enabled ? 1.f : 0.5f);
}
