#include "ui-components/WeaponModuleDetailComponent.hpp"

tgui::Container::Ptr WeaponModuleDetailComponent::buildLayout()
{
    auto&& panel = tgui::Panel::create();
    panel->setRenderer(
        tgui::Theme::getDefault()->getRenderer("AllCornersPanel"));
    panel->getRenderer()->setPadding({ 20.f, 20.f });

    auto&& nameGroup = WidgetBuilder::createRow(dic.sizer);
    panel->add(nameGroup, "WeaponModuleDetailNameGroup");
    nameGroup->add(
        WidgetBuilder::createTextLabel("", dic.sizer, "justify"_true),
        "WeaponModuleDetailName");

    auto&& imageGroup = tgui::Group::create({ "100%", "40%" });
    imageGroup->setPosition({ "0%", "WeaponModuleDetailNameGroup.height" });
    panel->add(imageGroup, "WeaponModuleDetailImageGroup");
    auto&& imagePanel = tgui::Panel::create({ "height", "80%" });
    TguiHelper::centerInParent(imagePanel);
    imageGroup->add(imagePanel, "WeaponModuleDetailImage");

    auto&& descriptionPanel = tgui::ScrollablePanel::create(
        { "100%",
          "100% - WeaponModuleDetailNameGroup.height - "
          "WeaponModuleDetailImageGroup.height" });
    TguiHelper::alignInParent(
        descriptionPanel,
        tgui::HorizontalAlignment::Left,
        tgui::VerticalAlignment::Bottom);
    descriptionPanel->getRenderer()->setPadding({ 20.f, 20.f });
    panel->add(descriptionPanel);

    auto&& description = WidgetBuilder::createTextLabel("", dic.sizer);
    descriptionPanel->add(description, "WeaponModuleDetailDescription");

    return panel;
}

void WeaponModuleDetailComponent::refreshDetail(const WeaponModule module)
{
    auto&& name = dic.gui.get<tgui::Label>("WeaponModuleDetailName");
    name->setText(dic.strings.getString(getModuleName(module)));

    auto&& image = dic.gui.get<tgui::Panel>("WeaponModuleDetailImage");
    image->getRenderer()->setTextureBackground(getWeaponModuleTexture(module));

    auto&& description =
        dic.gui.get<tgui::Label>("WeaponModuleDetailDescription");
    description->setText(dic.strings.getString(getModuleDescription(module)));
}
