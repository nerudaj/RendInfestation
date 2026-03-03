#pragma once

#include "input/InputKind.hpp"
#include "strings/StringId.hpp"
#include "strings/StringProvider.hpp"

class [[nodiscard]] InputKindToStringMapper final
{
public:
    explicit InputKindToStringMapper(const StringProvider& strings)
        : strings(strings)
    {
    }

    InputKindToStringMapper(InputKindToStringMapper&&) = delete;
    InputKindToStringMapper(const InputKindToStringMapper&) = delete;

public:
    [[nodiscard]] const CharType* inputKindToString(InputKind kind)
    {
        switch (kind)
        {
        case InputKind::BackButton:
            return strings.getString(StringId::InputKind_BackButton);
        case InputKind::ConfirmButton:
            return strings.getString(StringId::InputKind_ConfirmButton);
        case InputKind::PauseButton:
            return strings.getString(StringId::InputKind_PauseButton);
        case InputKind::CursorUp:
            return strings.getString(StringId::InputKind_CursorUp);
        case InputKind::CursorDown:
            return strings.getString(StringId::InputKind_CursorDown);
        case InputKind::CursorLeft:
            return strings.getString(StringId::InputKind_CursorLeft);
        case InputKind::CursorRight:
            return strings.getString(StringId::InputKind_CursorRight);
        case InputKind::MenuCycleLeft:
            return strings.getString(StringId::InputKind_MenuCycleLeft);
        case InputKind::MenuCycleRight:
            return strings.getString(StringId::InputKind_MenuCycleRight);
        case InputKind::Left:
            return strings.getString(StringId::InputKind_Left);
        case InputKind::Right:
            return strings.getString(StringId::InputKind_Right);
        case InputKind::Up:
            return strings.getString(StringId::InputKind_Up);
        case InputKind::Down:
            return strings.getString(StringId::InputKind_Down);
        case InputKind::AimLeft:
            return strings.getString(StringId::InputKind_AimLeft);
        case InputKind::AimRight:
            return strings.getString(StringId::InputKind_AimRight);
        case InputKind::AimUp:
            return strings.getString(StringId::InputKind_AimUp);
        case InputKind::AimDown:
            return strings.getString(StringId::InputKind_AimDown);
        case InputKind::Shoot:
            return strings.getString(StringId::InputKind_Shoot);
        case InputKind::SwitchWeapons:
            return strings.getString(StringId::InputKind_SwitchWeapons);
        default:
            return "--error--";
        }
    }

private:
    const StringProvider& strings;
};
