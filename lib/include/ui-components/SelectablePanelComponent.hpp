#pragma once

#include "misc/DependencyContainer.hpp"

class [[nodiscard]] SelectablePanelComponent final
{
public:
    static tgui::Panel::Ptr buildLayout(DependencyContainer& dic);
};
