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
    { Loading, "Loading" },
    { Clear, "Clear" },
    { Restart, "Restart" },
    { AreYouSure, "Are you sure?" },
    { Confirm, "Yes" },
    { Cancel, "No" },
    { DoYouWantToQuit, "Do you really want to leave the game?" },

    // Survival Level Select
    { SurvivalSetupTitle, "Survival" },
    { SelectMap, "Map" },
    { GameSetup, "Modifiers" },
    { BoonLocked, "- Locked -" },
    { BoonWave4, "Start at wave 4" },
    { BoonExtraWeapon, "Start with extra weapon" },
    { BoonEmergencyRevive, "Start with revive" },
    { StartingWeapon, "Starting weapon" },
    { StartingWeapon_None, "- None -" },
    { StartingWeapon_Random, "Random" },
    { StartingWeapon_Sweeper, "Sweeper" },
    { StartingWeapon_Landmine, "Landmines" },
    { StartingWeapon_Fragshot, "Fragshot" },
    { StartingWeapon_Painless, "Painless" },
    { StartingWeapon_WorstEver, "The worst one" },
    { StartingWeapon_Tower, "Tower" },
    { SurvivalMap01_Name, "Landing Bay" },
    { SurvivalMap01_Locked, "-- error locked level 01 --" },
    { SurvivalMap02_Name, "Botanical Sector" },
    { SurvivalMap02_Locked,
      "You need to beat wave 12 of Landing Bay to unlock this map." },
    { SurvivalMap03_Name, "Laboratories" },
    { SurvivalMap03_Locked,
      "You need to beat wave 12 of Botanical Sector to unlock this map." },

    // Options
    { Apply, "Apply" },
    { AudioVideoOptionsTab, "Audio / Video" },
    { GameplayOptionsTab, "Gameplay" },
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
    { EraseProgress, "Erase all progress" },
    { EraseProgressAreYouSure,
      "Are you sure you want to erase all your progress (unlockables, best "
      "scores, etc.)?" },
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
    { WeaponModule_PassthruAmmo, "Phaser" },
    { WeaponModule_BigBullet, "Heavy rounds" },
    { WeaponModule_Spikes, "Spikes" },
    { WeaponModule_Splitter, "Splitter" },
    { WeaponModule_Push, "Kickback" },
    { WeaponModule_Turret, "Turret" },
    { WeaponModule_Zap, "Zapper" },
    { WeaponModule_ExtraHealth, "Extra health" },
    { WeaponModule_ExtraGun, "Extra gun" },
    { WeaponModule_ShatteringRicochet, "Shattering ricochet" },
    { WeaponModule_PulseRounds, "Pulse rounds" },
    { WeaponModule_AutomaticShotgun, "Automatic Shotgun" },
    { InUse, "(in use)" },
    { Install, "Install" },
    { ConfigureLoadout, "Configure loadout" },
    { RemoveMod, "Clear" },

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
    { WeaponModuleDesc_Zap, "-- todo --" },
    { WeaponModuleDesc_ExtraHealth,
      "Heal to full health. Increases max health." },
    { WeaponModuleDesc_ExtraGun, "Gain an extra weapon slot." },
    { WeaponModuleDesc_ShatteringRicochet,
      "Projectile shatters into smaller projectiles upon hitting the wall. "
      "These can shatter again upon hitting another wall" },
    { WeaponModuleDesc_PulseRounds,
      "Projectiles gets bigger and more powerful, while being faster and with "
      "shorter cooldowns than Heavy rounds." },
    { WeaponModuleDesc_AutomaticShotgun,
      "Shoots 3 projectiles per shot really, really fast." },

    // Game
    { Interact, "[interact]" },
    { NewWeaponComboUnlocked, "New starting weapon unlocked!" },
    { Wave4SkipUnlocked, "You can now start from wave 4!" },
    { ExtraWeaponUnlocked, "Extra starting weapon unlocked!" },
    { NewMapUnlocked, "New map unlocked!" },
    { GotRevive, "Gained emergency revive!" },
    { StartingEmergencyReviveUnlocked, "Revive unlocked!" },

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

    // Note
    { Note1Title, "Cargo manifest" },
    { Note1,
      "From: Scorpio Corporation Station \"Europa\"\n"
      "To: Scorpio Corporation Space Ship \"Killer Whale\"\n"
      "Contents:\n"
      "\n"
      "  -DATA ENCRYPTED-\n"
      "  -LEVEL S CLEARANCE REQUIRED TO DECRYPT-" },
    { Note2Title, "Email" },
    { Note2,
      "from: vince.shall@kw.stinger\n"
      "to: logistics@kw.stinger, hr@kw.stinger\n"
      "subject: RE: Procedure violations\n"
      "\n"
      "Another shipment with classified cargo manifest arrived today. My job "
      "is monitoring incoming cargo, assess the safety, quarantine procedures, "
      "etc. I can't do my job when I don't have a clearance for the manifest. "
      "I am fine not having the clearance, but can you at least put in place a "
      "protocol for contacting someone how has it and can assess the "
      "situation? Thanks.\n"
      "\n"
      "Best regards,\n"
      "Vince Shall, Logistics Manager" },
    { Note3Title, "Gardening log #856" },
    { Note3,
      "New fertilizers have been deployed a month ago to the control group. "
      "Since then, we have observed both an improvement in growth rate (about "
      "9-11%), but we have also observed slight increase in mortality rate "
      "(about 3%) and one peculiar mutations. I would not be normally "
      "concerned about these numbers, but the way that one plant warbled in "
      "such a short timespan makes me worrier.\n"
      "\n"
      "We'll be deploying a second control group tomorrow." },
    { Note4Title, "NOTICE: Internal Personnel Update" },
    { Note4,
      "It is with deep regret that we inform you of the passing of Joe "
      "Wilkinson, a dedicated member of our logistics team. We extend our "
      "sincerest condolences and best wishes to Joe’s family and friends "
      "during this difficult time.\n"
      "\n"
      "While we mourn this loss, we must use this moment as a reminder that "
      "safety is a shared responsibility. All personnel are urged to maintain "
      "the required levels of vigilance and strictly adhere to all corporate "
      "safety protocols to prevent further accidents.\n"
      "\n"
      "Human Resources Department" },
    { Note5Title, "Autopsy report" },
    { Note5,
      "Subject: Joe Wilkinson\n"
      "\n"
      "The subject sustained extensive soft-tissue injuries of undetermined "
      "origin. Both lower extremities show multiple deep lacerations and "
      "abrasions, with areas of skin avulsion and exposed underlying "
      "tissue.The left shoulder and anterior chest exhibit burns consistent in "
      "appearance with thermal or electrical injury; a plasma discharge is one "
      "possible source.A single penetrating wound is present in the abdomen. "
      "The wound’s shape is consistent with a narrow, tapered object, though "
      "the instrument cannot be identified.\n"
      "\n"
      "The injuries are consistent with multiple mechanisms. The lower - "
      "extremity wounds may have been caused by an animal or other source of "
      "repeated trauma. The burns may have resulted from a directed energy "
      "discharge. The abdominal wound may have been inflicted by a pointed "
      "implement.These findings alone do not establish the sequence of events "
      "or the identity of any assailant.\n"
      "\n"
      "The circumstances remain unexplained. The subject was found in the "
      "Warehouse, and no witnesses have been identified." },
    { Note6Title, "Instant message" },
    { Note6,
      "fuck - have you heard? security just arrested JD for suspecting hes RU "
      "spy. fucking JD? this is nuts" },
    { Note7Title, "Modular Gun Testing Log" },
    { Note7,
      "So far we conducted testing of the following combinations:\n"
      " - Kickback, Heavy Rounds, Phaser\n"
      " - Spikes, Explosive Ammo, Spread x2\n"
      " - Spikes, Splitter, Spread x4\n"
      " - Cadence, Spread x2, Ricochet\n"
      " - Turret, Cadence, Kickback\n\n"
      "Lastly we tried Spikes, Explosive Ammo and Phaser, but that was the "
      "worst decision ever." },

    { Placeholder, "Placeholder" },
};
