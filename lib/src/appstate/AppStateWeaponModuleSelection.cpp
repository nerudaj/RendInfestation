#include "appstate/AppStateWeaponModuleSelection.hpp"
#include "appstate/CommonHandler.hpp"
#include "gui/GuiBuilderHelper.hpp"
#include "gui/ModalBuilder.hpp"

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

void AppStateWeaponModuleSelection::buildLayout()
{
    const auto&& options = ModalBuilderOptions {
        .modalSize = { "80%", "80%" },
        .titleBarColor = COLOR_MID_BLUE,
        .bodyScrollable = false,
        .bodyPadding = {},
        .bodyPanelClass = "UntexturedWidget",
        .navbarPanelClass = "BottomCutoutPanel",
    };

    auto&& builder = ModalBuilder(dic.strings, dic.sizer, dic.soundPlayer);
    auto&& modal = builder.openModal(StringId::SelectModule, options)
                       .withContent(buildModuleSelectionLayout())
                       .withBottomButton(StringId::Back, [&] { onCancel(); })
                       .withBottomButton(
                           StringId::RemoveMod,
                           [&] { onRemoveMod(); },
                           WidgetOptions {
                               .enabled = selectedModule != WeaponModule::None,
                           })
                       .withBottomButton(
                           StringId::Install,
                           [&] { onApply(); },
                           WidgetOptions {
                               .id = "ApplyButton",
                           })
                       .build();

    auto&& background = GuiBuilderHelper::createSemitransparentBlackPanel();
    dic.gui.add(background, "ModalBackground");
    background->onClick([] {});
    dic.gui.add(modal, "Modal");
}

tgui::Container::Ptr AppStateWeaponModuleSelection::buildModuleSelectionLayout()
{
    auto&& hbox = tgui::HorizontalLayout::create();

    auto&& scrollList = WidgetBuilder::createContainer<tgui::ScrollablePanel>({
        .className = "AllCornersPanelScrollable",
    });
    scrollList->getRenderer()->setPadding({ 20.f, 20.f });
    hbox->add(scrollList);

    auto&& moduleList = tgui::GrowVerticalLayout::create();
    scrollList->add(moduleList);

    for (auto&& module : scene.playerInventory.unlockedModules)
    {
        if (module == WeaponModule::None) continue;
        moduleList->add(
            buildModuleListEntryLayout(module), getModuleListEntryId(module));
    }

    hbox->add(moduleDetailComponent.buildLayout());

    return hbox;
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

    dic.soundPlayer.playPovSound(SoundId::UiButtonHover);
    dic.gui.get<tgui::Panel>(getModuleListEntryId(newSelectedModule))
        ->getRenderer()
        ->setBackgroundColor(COLOR_MID_BLUE);

    selectedModule = newSelectedModule;
    moduleDetailComponent.refreshDetail(selectedModule);

    auto&& enabled = !isInUseModule(newSelectedModule);
    auto&& button = dic.gui.get<tgui::Button>("ApplyButton");
    WidgetBuilder::setButtonEnabled(button, enabled);
}
