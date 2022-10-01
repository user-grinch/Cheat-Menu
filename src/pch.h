#pragma once
#pragma warning(disable:4503 4244 4005)

#include <d3d9.h>
#include <d3d11.h>
#include <d3d11Shader.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <windows.h>

#include <plugin.h>
#include <CCamera.h>
#include <CClock.h>
#include <CCivilianPed.h>
#include <CGangs.h>
#include <CHud.h>
#include <CMenuManager.h>
#include <CModelInfo.h>
#include <RenderWare.h>
#include <CStats.h>
#include <CStreaming.h>
#include <CTheScripts.h>
#include <CTimer.h>
#include <CTimeCycle.h>
#include <CTrain.h>
#include <CWeather.h>
#include <CWorld.h>
#include <extensions/ScriptCommands.h>
#include <extensions/Screen.h>
#include <extensions/Paths.h>

#ifndef GTA3
#include <CBike.h>
#include <cHandlingDataMgr.h>
#endif

#ifdef GTASA
#include <CCheat.h>
#include <CClothes.h>
#include <CCutsceneMgr.h>
#include <CRadar.h>
#include <CShadows.h>
#include <eVehicleClass.h>
#include <CGangWars.h>
#include <CZoneInfo.h>
#include <CTheZones.h>
#endif

#include "fla/IDaccess.h"
#include "imgui/imgui.h"

#include "defines.h"
#include "utils/log.h"
#include "utils/datastore.h"
#include "utils/hotkeys.h"
#include "utils/resourcestore.h"
#include "utils/fontmgr.h"
#include "utils/locale.h"
#include "utils/util.h"

#define TEXT(x) Locale::GetText(x).c_str()
#define TEXT_S(x) Locale::GetText(x)

using namespace plugin;

enum class eRenderer
{
    DirectX9,
    DirectX11,
    Unknown
};

extern eRenderer gRenderer;
extern DataStore gConfig;

using str = std::string;
using fArgNone_t = std::function<void()>;
using fArg1_t = std::function<void(str&)>;
using fArg3_t = std::function<void(str&, str&, str&)>;
using fRtnArg1_t = std::function<std::string(str&)>;
using fRtnBoolArg1_t = std::function<bool(str&)>;

#define fArg3Wrapper(x) [](str& a, str& b, str& c){x(a, b, c);}
#define fArgWrapper(x) [](str& a){x(a);}
#define fRtnArgWrapper(x) [](str& a){return x(a);}
#define fArgNoneWrapper(x) [](){x();}

#ifdef GTASA
extern const char* taskNames[1802];
#endif
extern Hotkey aimSkinChanger;
extern Hotkey freeCam;
extern Hotkey freeCamForward;
extern Hotkey freeCamBackward;
extern Hotkey freeCamLeft;
extern Hotkey freeCamRight;
extern Hotkey freeCamTeleport;
extern Hotkey commandWindow;
extern Hotkey fixVeh;
extern Hotkey unflipVeh;
extern Hotkey godMode;
extern Hotkey menuOpen;
extern Hotkey quickSceenShot;
extern Hotkey quickTeleport;
extern Hotkey teleportMarker;
extern Hotkey vehEngine;
extern Hotkey vehInstantStart;
extern Hotkey vehInstantStop;

#ifdef GTASA
extern ResourceStore gTextureList;
#endif