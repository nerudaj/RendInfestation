#include "appstate/AppStateAreYouSure.hpp"
#include "appstate/CommonHandler.hpp"
#include "gui/GuiBuilderHelper.hpp"
#include "gui/ModalBuilder.hpp"

constexpr const char* MODAL_ID = "AreYouSureModal";

AppStateAreYouSure::AppStateAreYouSure(
    dgm::App& app,
    DependencyContainer& dic,
    StringId descriptionStringId,
    bool popOnConfirm,
    std::function<void(void)> onConfirm)
    : dgm::AppState(
          app,
          dgm::AppStateConfig {
              .shouldDrawUnderlyingState = true,
          })
    , dic(dic)
    , onConfirm(onConfirm)
{
    buildLayout(descriptionStringId, popOnConfirm);
}

AppStateAreYouSure::~AppStateAreYouSure()
{
    auto&& modal = dic.gui.get<tgui::ChildWindow>(MODAL_ID);
    modal->close();
    dic.gui.remove(modal);
    dic.gui.remove(dic.gui.get<tgui::Panel>("ModalBackground"));
}

void AppStateAreYouSure::input()
{
    CommonHandler::handleInput(
        app,
        dic,
        dic.settings.input,
        CommonHandlerOptions {
            .disableGoBack = true,
        });
}

void AppStateAreYouSure::update() {}

void AppStateAreYouSure::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateAreYouSure::buildLayout(
    StringId descriptionStringId, bool popOnConfirm)
{
    auto&& getLayoutSize = []()
    {
#ifdef ANDROID
        return tgui::Layout2d { "60%", "40%" };
#else
        return tgui::Layout2d { "40%", "30%" };
#endif
    };

    auto&& modal =
        ModalBuilder(dic.strings, dic.sizer, dic.soundPlayer)
            .openModal(
                StringId::AreYouSure,
                GuiBuilderHelper::getCommonModalBuilderOptions(getLayoutSize()))
            .withContent(buildContentLayout(descriptionStringId))
            .withBottomButton(
                StringId::Confirm,
                [this, popOnConfirm]
                {
                    onConfirm();
                    if (popOnConfirm) app.popState();
                })
            .withBottomButton(StringId::Cancel, [&] { onCancel(); })
            .build();

    auto&& background = GuiBuilderHelper::createSemitransparentBlackPanel();
    dic.gui.add(background, "ModalBackground");
    background->onClick([] {});
    dic.gui.add(modal, MODAL_ID);
}

tgui::Container::Ptr
AppStateAreYouSure::buildContentLayout(StringId descriptionStringId)
{
    auto&& group = tgui::Group::create();

    auto&& label = WidgetBuilder::createTextLabel(
        dic.strings.getString(descriptionStringId), dic.sizer, "justify"_false);

    group->add(label);

    return group;
}

void AppStateAreYouSure::onCancel()
{
    app.popState();
}
