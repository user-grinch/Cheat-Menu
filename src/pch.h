#pragma once
#pragma warning(disable:4503 4244 4005)

#define INPUT_BUFFER_SIZE 64
#define SPAWN_PED_LIMIT 20
#define DISCORD_INVITE "https://discord.gg/ZzW7kmf"
#define GITHUB_LINK "https://github.com/user-grinch/Cheat-Menu"

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
#include "CShadows.h"
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
#include "eVehicleClass.h"
#include "extensions/Paths.h"

#include "../Depend/fla/IDaccess.h"
#include "../Depend/imgui/imgui.h"
#include "../Depend/imgui/imgui_internal.h"
#include "../Depend/imgui/imgui_impl_dx9.h"
#include "../Depend/imgui/imgui_impl_dx11.h"
#include "../Depend/imgui/imgui_impl_win32.h"

#include "MoreEvents.h"
#include "Json.h"
#include "VKeys.h"

// Globals
using CallbackTable = std::vector<std::pair<std::string, void(*)()>>;
using namespace plugin;
namespace fs = std::filesystem;

enum Renderer
{
	Render_DirectX9,
	Render_DirectX11,
	Render_Unknown
};

struct Globals
{
	inline static std::string m_HeaderId;
	inline static ImVec2 m_fMenuSize = ImVec2(screen::GetScreenWidth() / 4, screen::GetScreenHeight() / 1.2);
	inline static ImVec2 m_fScreenSize = ImVec2(-1, -1);
	inline static bool m_bShowMenu = false;
	inline static bool m_bInit;
	inline static Renderer renderer = Render_Unknown;
	inline static void* device;
};

extern std::ofstream flog;
extern CJson config;

struct HotKeyData
{
	int m_key1;
	int m_key2;
	bool m_bPressed;
};

struct RwD3D9Raster
{
	union
	{
		IDirect3DTexture9* texture;
		IDirect3DSurface9* surface;
	};
	unsigned char* palette;
	unsigned char alpha;
	unsigned char cubeTextureFlags; /* 0x01 IS_CUBEMAP_TEX */
	unsigned char textureFlags; /* 0x10 IS_COMPRESSED */
	unsigned char lockedLevel;
	IDirect3DSurface9* lockedSurface;
	D3DLOCKED_RECT lockedRect;
	D3DFORMAT format;
	IDirect3DSwapChain9* swapChain;
	HWND* hwnd;
};

struct RwRasterEx : public RwRaster
{
	RwD3D9Raster *renderResource;
};

struct STextureStructure
{
	std::string m_FileName;
	std::string m_CategoryName;
	RwTexture *m_pRwTexture = nullptr;
};

struct SSearchData
{
	CJson m_Json;
	RwTexDictionary txd;
	ImGuiTextFilter m_Filter = "";
	std::vector<std::string> m_Categories = {"All"};
	std::string m_Selected = "All";
	std::vector<std::unique_ptr<STextureStructure>> m_ImagesList;
};
