#pragma once

class Menu
{
private:
	struct overlay
	{
		inline static bool bCoord = false;
		inline static bool bFPS = false;
		inline static int mFPS = 0;
		inline static bool bLocName = false;
		inline static bool bTransparent = false;
		inline static bool bVehHealth = false;
		inline static bool bVehSpeed = false;
		inline static bool bCpuUsage = false;
		inline static float fCpuUsage = 0.0f;
		inline static bool bMemUsage = false;
		inline static float fMemUsage = 0.0f;
		inline static std::vector<std::string> posNames = { "Custom", "Top left", "Top right", "Bottom left", "Bottom right" };
		inline static int mSelectedPos = 4; // TODO: Create Enum
		inline static float fPosX = 0.0f;
		inline static float fPosY = 0.0f;
		inline static size_t mInterval = 1000;
		inline static size_t mLastInterval = 0;
		inline static int mTotalRam = 0;
	};
public:
	struct hotkeys
	{
		inline static HotKeyData aim_skin_changer;
		inline static HotKeyData freecam;
		inline static HotKeyData command_window;
		inline static HotKeyData fix_veh;
		inline static HotKeyData flip_veh;
		inline static HotKeyData free_cam_tp_player{VK_RETURN,VK_RETURN};
		inline static HotKeyData god_mode;
		inline static HotKeyData menu_open;
		inline static HotKeyData quick_ss;
		inline static HotKeyData quick_tp;
		inline static HotKeyData veh_engine;
		inline static HotKeyData veh_instant_start;
		inline static HotKeyData veh_instant_stop;
	};
	struct commands
	{
		inline static bool show_menu = false;
		inline static char input_buffer[INPUT_BUFFER_SIZE] = "";
	};

	Menu();
	static void Draw();
	static void DrawOverlay();
	static void DrawShortcutsWindow();
	static void ProcessCommands();
};

