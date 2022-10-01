#include "pch.h"
eRenderer gRenderer = eRenderer::Unknown;
DataStore gConfig = DataStore(FILE_NAME, true);

#ifdef GTASA
ResourceStore gTextureList { "misc", eResourceType::TYPE_IMAGE, ImVec2(100, 80) };
#endif
Hotkey aimSkinChanger {VK_RETURN, VK_RETURN, "AimSkinChanger"};
Hotkey freeCam {VK_F6, VK_F6, "Freecam.Toggle"};
Hotkey freeCamForward {VK_KEY_I, VK_KEY_I, "Freecam.Forward"};;
Hotkey freeCamBackward {VK_KEY_K, VK_KEY_K, "Freecam.Backward"};;
Hotkey freeCamLeft {VK_KEY_J, VK_KEY_J, "Freecam.Left"};;
Hotkey freeCamRight {VK_KEY_L, VK_KEY_L, "Freecam.Right"};
Hotkey freeCamTeleport {VK_RETURN, VK_RETURN, "Freecam.Teleport"};
Hotkey commandWindow {VK_LCONTROL, VK_KEY_C, "CommandWindowToggle"};;
Hotkey fixVeh {VK_NONE, VK_NONE, "Vehicle.Fix"};
Hotkey unflipVeh {VK_NONE, VK_NONE, "Vehicle.Flip"};
Hotkey godMode {VK_NONE, VK_NONE, "GodMode"};
Hotkey menuOpen {VK_LCONTROL, VK_KEY_M, "MenuToggle"};
Hotkey quickSceenShot {VK_F5, VK_F5, "QuickScreenshot"};
Hotkey quickTeleport {VK_LSHIFT, VK_KEY_Q, "QuickTeleport"};
Hotkey teleportMarker {VK_KEY_X, VK_KEY_Y, "TeleportShortcut"};
Hotkey vehEngine {VK_NONE, VK_NONE, "Vehicle.EngineToggle"};
Hotkey vehInstantStart {VK_NONE, VK_NONE, "Vehicle.InstantStart"};
Hotkey vehInstantStop {VK_NONE, VK_NONE, "Vehicle.InstantStop"};