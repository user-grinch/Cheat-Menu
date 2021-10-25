#pragma once
#pragma warning(disable:4503 4244 4005)

#define INPUT_BUFFER_SIZE 64
#define SPAWN_PED_LIMIT 20
#define DISCORD_INVITE "https://discord.gg/ZzW7kmf"
#define GITHUB_LINK "https://github.com/user-grinch/Cheat-Menu"

#ifdef GTASA
#define BY_GAME(sa, vc, iii) sa
#elif GTAVC
#define BY_GAME(sa, vc, iii) vc
#elif GTA3
#define BY_GAME(sa, vc, iii) iii
#endif

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
#include <CTheZones.h>
#include <CZoneInfo.h>
#endif

#ifdef GTASA
#include <CCheat.h>
#include <CClothes.h>
#include <CCutsceneMgr.h>
#include <CRadar.h>
#include <CShadows.h>
#include <eVehicleClass.h>
#include <CGangWars.h>
#endif

#include "../depend/fla/IDaccess.h"
#include "../depend/imgui/imgui.h"

#include "json.h"
#include "hotkeys.h"
#include "vKeys.h"
#include "resourcestore.h"

using CallbackTable = std::vector<std::pair<std::string, void(*)()>>;
using namespace plugin;

enum eRenderer
{
	Render_DirectX9,
	Render_DirectX11,
	Render_Unknown
};

static eRenderer gRenderer = Render_Unknown;
static std::ofstream gLog = std::ofstream("CheatMenu.log");
// why doesn't this work?
// inline CJson gConfig = CJson("config");
extern CJson gConfig;

// Fix function clashes
static void SetHelpMessage(const char *message, bool b1, bool b2, bool b3)
{
#if GTASA
	CHud::SetHelpMessage(message, b1, b2, b3);
#elif GTAVC
	CHud::SetHelpMessage(message, b1, b2);
#else // GTA3
	const size_t cSize = strlen(message)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, message, cSize);
	CHud::SetHelpMessage((wchar_t*)wc, b1);
#endif
}