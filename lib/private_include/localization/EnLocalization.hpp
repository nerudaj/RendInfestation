#pragma once

#include "strings/StringId.hpp"
#include "types/StringTypes.hpp"

using enum StringId;

const std::map<StringId, StringViewType> EN_LOCALIZATION = {
    { GameTitle, "Rend: Infestation" },

    { PlayButton, "Play" },
    { SurvivalButton, "Survival" },
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
    { RenderColliders, "Render colliders" },
    { SoundVolume, "Sound volume" },
    { MusicVolume, "Music volume" },
    { GamepadDeadzone, "Gamepad deadzone" },
    { CursorSpeed, "Cursor speed" },
    { ShowTouchControls, "Show touch controls" },
    { CameraFollowsCrosshair, "Camera follows crosshair" },
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
    { InputKind_Interact, "Interact" },

    // Pause
    { Resume, "Resume game" },
    { BackToMenu, "Back to main menu" },

    // WeaponModification
    { ChooseBonus, "Pick a bonus" },
    { SelectModule, "Pick module to install" },
    { Weapon1Title, "Weapon 1" },
    { Weapon2Title, "Weapon 2" },
    { WeaponModuleSlot1, "Module 1" },
    { WeaponModuleSlot2, "Module 2" },
    { WeaponModuleSlot3, "Module 3" },
    { WeaponModule_None, "None" },
    { WeaponModule_SpreadBarrelx2, "Spread x2" },
    { WeaponModule_SpreadBarrel, "Spread x4" },
    { WeaponModule_CadenceBarrel, "Cadence Barrel" },
    { WeaponModule_ExplosiveAmmo, "Explosive Ammo" },
    { WeaponModule_Ricochet, "Ricochet" },
    { WeaponModule_PassthruAmmo, "Passthru Ammo" },
    { WeaponModule_BigBullet, "Big Bullet" },
    { WeaponModule_Spikes, "Spikes" },
    { WeaponModule_Splitter, "Splitter" },
    { WeaponModule_ExtraHealth, "Extra health" },
    { WeaponModule_ExtraGun, "Extra gun" },

    // WeaponModDec
    { WeaponModuleDesc_None, "No description" },
    { WeaponModuleDesc_SpreadBarrelx4, "Shoot 4 projectiles at once." },
    { WeaponModuleDesc_SpreadBarrelx2, "Shoot 2 projectiles at once." },
    { WeaponModuleDesc_CadenceBarrel,
      "Decreases delay between shots, decreases damage of each projectile." },
    { WeaponModuleDesc_Ricochet, "Projectiles ricochet off the walls." },
    { WeaponModuleDesc_PassthruAmmo,
      "Projectiles penetrate through enemies. Hitting an enemy no longer "
      "counts as impact." },
    { WeaponModuleDesc_BigBullet,
      "Projectiles get bigger, slower, and more powerful. Delay between shots "
      "is increased." },
    { WeaponModuleDesc_Splitter,
      "Projectiles shatter into smaller projectiles upon impact." },
    { WeaponModuleDesc_Spikes,
      "Projectiles transform into trip spikes that remain lying on the "
      "floor." },
    { WeaponModuleDesc_Push,
      "Projectiles pack high kinetic force, pushing enemies into pits." },
    { WeaponModuleDesc_ExplosiveAmmo, "Projectiles explode upon impact." },
    { WeaponModuleDesc_Turret,
      "Projectiles transform into mobile turrets. All other equipped modules "
      "affect the turret behavior." },
    { WeaponModuleDesc_ExtraHealth,
      "Heal to full health. Increases max health." },
    { WeaponModuleDesc_ExtraGun, "Gain an extra weapon slot." },

    // Game
    { Interact, "[interact]" },

    // Survival progression
    { NewWeaponModuleUnlocked, "New weapon module unlocked!" },
    { GameOver, "Game Over" },
    { GameScore, "Your score" },
    { EnemiesRouted, "Enemies routed" },
    { ShotsFired, "Shots fired" },
    { WavesBeaten, "Waves beaten" },
    { NewBestScore, "(new best!)" },
    { PreviousBestScore, "Previous best score" },
    { CurrentBestScore, "Current best score" },

    { Placeholder, "Placeholder" },
};
