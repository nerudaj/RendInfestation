#pragma once

#include "strings/StringId.hpp"
#include "types/StringTypes.hpp"

using enum StringId;

const std::map<StringId, StringViewType> EN_LOCALIZATION = {
    { PlayButton, "Play" },
    { Options, "Options" },
    { PauseTitle, "Game paused" },
    { ExitButton, "Exit" },
    { Back, "Back" },

    // Options
    { Apply, "Apply" },
    { VideoOptionsTab, "Video" },
    { AudioOptionsTab, "Audio" },
    { InputOptionsTab, "Input" },
    { BindingsOptionsTab, "Bindings" },
    { EnableFullscreen, "Fullscreen" },
    { SetResolution, "Resolution" },
    { SetUiScale, "Set UI scale" },
    { SetTheme, "Set theme" },
    { SoundVolume, "Sound volume" },
    { MusicVolume, "Music volume" },
    { GamepadDeadzone, "Gamepad deadzone" },
    { CursorSpeed, "Cursor speed" },
    { ShowTouchControls, "Show touch controls" },
    { ReleaseAllInputs, "Release all inputs" },
#ifdef ANDROID
    { PressAKeyOrEscape,
      "Press a key\nor click outside of\nthis message to cancel" },
#else
    { PressAKeyOrEscape, "Press a key\nor Escape to cancel" },
#endif
    { BindingHeadingAction, "Action" },
    { BindingHeadingKMB, "Keyboard/Mouse" },
    { BindingsHeadingGamepad, "Gamepad" },

    // InputKind
    { InputKind_BackButton, "Back" },
    { InputKind_ConfirmButton, "Confirm" },
    { InputKind_PauseButton, "Pause" },
    { InputKind_CursorUp, "Cursor up" },
    { InputKind_CursorDown, "Cursor down" },
    { InputKind_CursorLeft, "Cursor left" },
    { InputKind_CursorRight, "Cursor right" },
    { InputKind_MenuCycleLeft, "Menu cycle left" },
    { InputKind_MenuCycleRight, "Menu cycle right" },
    { InputKind_Left, "Run left" },
    { InputKind_Right, "Run right" },
    { InputKind_Up, "Run up" },
    { InputKind_Down, "Run down" },
    { InputKind_AimLeft, "Aim left" },
    { InputKind_AimRight, "Aim right" },
    { InputKind_AimUp, "Aim up" },
    { InputKind_AimDown, "Aim down" },
    { InputKind_Shoot, "Shoot" },
    { InputKind_SwitchWeapons, "Switch weapons" },

    // Pause
    { Resume, "Resume game" },
    { BackToMenu, "Back to main menu" },
};
