#pragma once
#pragma warning(disable:4503 4244)

#define INPUT_BUFFER_SIZE 64
#define SPAWN_PED_LIMIT 20
#define MENU_VERSION "2.5-beta"
#define BUILD_NUMBER "20201209"
#define STB_IMAGE_IMPLEMENTATION

#include <d3d9.h>
#include <d3d11.h>
#include <d3d11Shader.h>
#include <D3dx9tex.h>
#include <D3DX11tex.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <stdlib.h>     
#include <sstream>
#include <vector>
#include <windows.h>

#include "plugin.h"
#include "CCamera.h"
#include "CCheat.h"
#include "CClothes.h"
#include "CClock.h"
#include "CCivilianPed.h"
#include "CCutsceneMgr.h"
#include "CGangs.h"
#include "CGangWars.h"
#include "cHandlingDataMgr.h"
#include "CHud.h"
#include "CMenuManager.h"
#include "CModelInfo.h"
#include "CRadar.h"
#include "RenderWare.h"
#include "CStats.h"
#include "CStreaming.h"
#include "CTheScripts.h"
#include "CTheZones.h"
#include "CTimer.h"
#include "CTimeCycle.h"
#include "CWeather.h"
#include "CWorld.h"
#include "extensions/ScriptCommands.h"
#include "extensions/Screen.h"
#include "eVehicleClass.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_dx9.h"
#include "external/imgui/imgui_impl_dx11.h"
#include "external/imgui/imgui_impl_win32.h"

#include "Events.h"
#include "Json.h"
#include "VKeys.h"

// Globals
typedef std::vector<std::pair<std::string, void(*)(void)>> unsortedMap;
using namespace plugin;
namespace fs = std::experimental::filesystem;

enum Renderer
{
	Render_DirectX9,
	Render_DirectX10,
	Render_DirectX11,
	Render_DirectX12,
	Render_OpenGL,
	Render_Unknown,
	Render_Vulkan,
};

struct Globals
{
	static std::string header_id;
	static int last_key_timer;
	static ImVec2 menu_size;
	static std::string menu_title;
	static ImVec2 font_screen_size;
	static bool show_menu;
	static bool init_done;
	static Renderer renderer;
	static ID3D11Device* device11;
};

struct TextureStructure
{
	std::string file_name;
	std::string category_name;
	PDIRECT3DTEXTURE9 texture9;
	ID3D11ShaderResourceView* texture11;
};

extern CJson config;
extern std::ofstream flog;

// Random cheats
//extern std::random_device rd; 
//extern std::mt19937 gen;
//extern std::uniform_int_distribution<> cheat_id;

#include "Ui.h"
#include "Util.h"