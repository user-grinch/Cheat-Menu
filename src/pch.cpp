#include "pch.h"
eRenderer gRenderer = Render_Unknown;
std::ofstream gLog = std::ofstream("CheatMenu.log");
CJson gConfig = CJson("config");

Hotkey aimSkinChanger;
Hotkey freeCam;
Hotkey commandWindow;
Hotkey fixVeh;
Hotkey flipVeh;
Hotkey freeCamTeleportPlayer;
Hotkey godMode;
Hotkey menuOpen;
Hotkey quickSceenShot;
Hotkey quickTeleport;
Hotkey vehEngine;
Hotkey vehInstantStart;
Hotkey vehInstantStop;