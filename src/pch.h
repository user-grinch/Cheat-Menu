#pragma once
#pragma warning(disable:4503 4244 4005)

#define INPUT_BUFFER_SIZE 64
#define SPAWN_PED_LIMIT 20
#define DISCORD_INVITE "https://discord.gg/ZzW7kmf"
#define GITHUB_LINK "https://github.com/user-grinch/Cheat-Menu"

#ifdef GTASA
#define BY_GAME(sa, vc) sa
#elif GTAVC
#define BY_GAME(sa, vc) vc
#endif

#include "header_includes.h"

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

// Common defines
static void SetHelpMessage(const char *message, bool b1, bool b2, bool b3)
{
#if GTAVC
    CHud::SetHelpMessage(message, b1, b2);
#elif GTASA
	CHud::SetHelpMessage(message, b1, b2, b3);
#endif
}