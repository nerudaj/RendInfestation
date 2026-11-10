#include "appstate/AppStateNoteReader.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "gui/ModalBuilder.hpp"
#include "gui/TguiHelper.hpp"

AppStateNoteReader::AppStateNoteReader(
    dgm::App& app, DependencyContainer& dic, int noteId)
    : dgm::AppState(
          app,
          dgm::AppStateConfig {
              .shouldDrawUnderlyingState = true,
          })
    , dic(dic)
{
    buildLayout(noteId);
}

void AppStateNoteReader::input()
{
    CommonHandler::handleInput(app, dic, dic.settings.input);
}

void AppStateNoteReader::update() {}

void AppStateNoteReader::draw()
{
    dic.gui.draw();
    dic.virtualCursor.draw();
}

void AppStateNoteReader::buildLayout(int noteId)
{
    auto&& panel = WidgetBuilder::createContainer<tgui::ScrollablePanel>(
        { "45%", "90%" },
        WidgetOptions {
            .className = "AllCornersPanelScrollable",
        });
    panel->getRenderer()->setPadding({ 20.f, 20.f });
    panel->getRenderer()->setScrollbarWidth(100.f);

    auto&& getStringId = [](StringId first, int offset)
    { return static_cast<StringId>(std::to_underlying(first) + 2 * offset); };

    auto&& getString = [&](StringId first, int offset)
    { return dic.strings.getString(getStringId(first, offset)); };

    auto&& label = WidgetBuilder::createTextLabel(
        getString(StringId::Note1, noteId), dic.sizer);
    label->setVerticalAlignment(tgui::VerticalAlignment::Top);

    panel->add(label);

    auto&& builder = ModalBuilder(dic.strings, dic.sizer, dic.soundPlayer);

    dic.gui.rebuildWith(
        builder
            .openModal(
                getStringId(StringId::Note1Title, noteId), { "45%", "90%" })
            .withContent(panel, "BottomCutoutPanel")
            .withBottomButton(
                StringId::Back,
                [&] { app.popState(Messaging::serialize<PopIfNotGame>()); })
            .build());
}
