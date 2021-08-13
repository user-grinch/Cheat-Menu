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

#include "plugin.h"
#include "CBike.h"
#include "CCamera.h"
#include "CClock.h"
#include "CCivilianPed.h"
#include "CGangs.h"
#include "cHandlingDataMgr.h"
#include "CHud.h"
#include "CMenuManager.h"
#include "CModelInfo.h"
#include "RenderWare.h"
#include "CStats.h"
#include "CStreaming.h"
#include "CTheScripts.h"
#include "CTheZones.h"
#include "CTimer.h"
#include "CTimeCycle.h"
#include "CTrain.h"
#include "CWeather.h"
#include "CWorld.h"
#include "extensions/ScriptCommands.h"
#include "extensions/Screen.h"
#include "extensions/Paths.h"

#ifdef GTASA
#include "CCheat.h"
#include "CClothes.h"
#include "CCutsceneMgr.h"
#include "CRadar.h"
#include "CShadows.h"
#include "eVehicleClass.h"
#include "CGangWars.h"
#endif

#include "../Depend/fla/IDaccess.h"
#include "../Depend/imgui/imgui.h"
#include "../Depend/imgui/imgui_internal.h"
#include "../Depend/imgui/imgui_impl_dx9.h"
#include "../Depend/imgui/imgui_impl_dx11.h"
#include "../Depend/imgui/imgui_impl_win32.h"

#include "MoreEvents.h"
#include "Json.h"
#include "VKeys.h"
#include "ResourceStore.h"