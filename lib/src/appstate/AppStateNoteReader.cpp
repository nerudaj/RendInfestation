#include "appstate/AppStateNoteReader.hpp"
#include "appstate/CommonHandler.hpp"
#include "appstate/Messaging.hpp"
#include "gui/GuiBuilderHelper.hpp"
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
    auto&& getLayoutSize = []()
    {
#ifdef ANDROID
        return tgui::Layout2d { "60%", "90%" };
#else
        return tgui::Layout2d { "45%", "80%" };
#endif
    };

    auto&& getStringId = [](StringId first, int offset)
    { return static_cast<StringId>(std::to_underlying(first) + 2 * offset); };

    const auto&& options = GuiBuilderHelper::getCommonModalBuilderOptions(
        getLayoutSize(), "scrollable"_true);

    dic.gui.rebuildWith(
        ModalBuilder(dic.strings, dic.sizer, dic.soundPlayer)
            .openModal(getStringId(StringId::Note1Title, noteId), options)
            .withContent(
                buildContentLayout(getStringId(StringId::Note1, noteId)))
            .withBottomButton(
                StringId::Back,
                [&] { app.popState(Messaging::serialize<PopIfNotGame>()); })
            .build());
}

tgui::Container::Ptr AppStateNoteReader::buildContentLayout(StringId stringId)
{
    auto&& content = tgui::Group::create();

    auto&& label = WidgetBuilder::createTextLabel(
        dic.strings.getString(stringId), dic.sizer);
    label->setVerticalAlignment(tgui::VerticalAlignment::Top);
    content->add(label);

    return content;
}
