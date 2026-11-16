#include "appstate/AppStateUnlocksSummary.hpp"
#include "appstate/CommonHandler.hpp"
#include "gui/GuiBuilderHelper.hpp"
#include "gui/ModalBuilder.hpp"

constexpr const char* MODAL_ID = "AreYouSureModal";
constexpr const char* BACKGROUND_ID = "ModalBackground";

AppStateUnlocksSummary::AppStateUnlocksSummary(
    dgm::App& app, DependencyContainer& dic, const Ledger& ledger)
    : dgm::AppState(app), dic(dic)
{
    buildLayout(ledger);
}

AppStateUnlocksSummary::~AppStateUnlocksSummary()
{
    auto&& modal = dic.gui.get<tgui::ChildWindow>(MODAL_ID);
    modal->close();
    dic.gui.remove(modal);
    dic.gui.remove(dic.gui.get<tgui::Panel>(BACKGROUND_ID));
}

void AppStateUnlocksSummary::input()
{
    CommonHandler::handleInput(app, dic, dic.settings.input);
}

void AppStateUnlocksSummary::update() {}

void AppStateUnlocksSummary::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateUnlocksSummary::buildLayout(const Ledger& ledger)
{
    auto&& getLayoutSize = []()
    {
#ifdef ANDROID
        return tgui::Layout2d { "60%", "80%" };
#else
        return tgui::Layout2d { "50%", "70%" };
#endif
    };

    auto&& modal =
        ModalBuilder(dic.strings, dic.sizer, dic.soundPlayer)
            .openModal(
                StringId::StuffUnlocked,
                GuiBuilderHelper::getCommonModalBuilderOptions(getLayoutSize()))
            .withContent(buildContentLayout(ledger))
            .withBottomButton(StringId::Back, [this] { app.popState(); })
            .build();

    auto&& background = GuiBuilderHelper::createSemitransparentBlackPanel();
    dic.gui.add(background, BACKGROUND_ID);
    background->onClick([] {});
    dic.gui.add(modal, MODAL_ID);
}

tgui::Container::Ptr
AppStateUnlocksSummary::buildContentLayout(const Ledger& ledger)
{
    if (ledger.empty()) return tgui::Group::create();

    auto builder = dic.guiBuilderFactory.createTableBuilder().withNoHeading();

    for (auto&& line : ledger)
    {
        builder.addRow({ WidgetBuilder::createTextLabel(
            dic.strings.getString(line), dic.sizer, "justify"_true) });
    }

    return builder.build();
}
