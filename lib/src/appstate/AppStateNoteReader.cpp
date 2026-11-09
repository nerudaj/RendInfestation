#include "appstate/AppStateNoteReader.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
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
    auto&& panel = tgui::ScrollablePanel::create({ "45%", "90%" });
    panel->getRenderer()->setPadding({ 20.f, 20.f });
    panel->getRenderer()->setScrollbarWidth(100.f);

    TguiHelper::centerInParent(panel);

    const auto stringId =
        static_cast<StringId>(std::to_underlying(StringId::Note1) + noteId);
    panel->add(WidgetBuilder::createTextLabel(
        dic.strings.getString(stringId), dic.sizer));

    dic.gui.rebuildWith(panel);
}
