#pragma once

// localizations can be found under private_include/localizations

enum class [[nodiscard]] StringId
{
    GameTitle,

    PlayButton,
    SurvivalButton,
    Options,
    PauseTitle,
    ExitButton,
    Back,

    // Options
    Apply,
    VideoOptionsTab,
    AudioOptionsTab,
    InputOptionsTab,
    BindingsOptionsTab,
    EnableFullscreen,
    SetResolution,
    SetUiScale,
    SetTheme,
    RenderColliders,
    SoundVolume,
    MusicVolume,
    GamepadDeadzone,
    CursorSpeed,
    ShowTouchControls,
    CameraFollowsCrosshair,
    ReleaseAllInputs,
    PressAKeyOrEscape,
    BindingHeadingAction,
    BindingHeadingKMB,
    BindingsHeadingGamepad,

    // InputKind
    InputKind_BackButton,
    InputKind_ConfirmButton,
    InputKind_PauseButton,
    InputKind_CursorUp,
    InputKind_CursorDown,
    InputKind_CursorLeft,
    InputKind_CursorRight,
    InputKind_MenuCycleLeft,
    InputKind_MenuCycleRight,
    InputKind_Left,
    InputKind_Right,
    InputKind_Up,
    InputKind_Down,
    InputKind_AimLeft,
    InputKind_AimRight,
    InputKind_AimUp,
    InputKind_AimDown,
    InputKind_Shoot,
    InputKind_SwitchWeapons,

    // Pause
    Resume,
    BackToMenu,

    // WeaponModification
    WeaponModification,
    SelectModule,
    Weapon1Title,
    Weapon2Title,
    WeaponModuleSlot1,
    WeaponModuleSlot2,
    WeaponModuleSlot3,
    WeaponModule_None,
    WeaponModule_SpreadBarrelx2,
    WeaponModule_SpreadBarrel,
    WeaponModule_CadenceBarrel,
    WeaponModule_ExplosiveAmmo,
    WeaponModule_Ricochet,
    WeaponModule_PassthruAmmo,
    WeaponModule_BigBullet,
    WeaponModule_Spikes,
    WeaponModule_Splitter,

    // Survival progression
    NewWeaponModuleUnlocked,
    GameOver,
    GameScore,

    MaxId, // Make sure this one is always the last!
};
