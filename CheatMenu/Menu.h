#pragma once

class Menu
{
private:

	enum DISPLAY_POS
	{
		CUSTOM,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT
	};

	struct m_Overlay
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
		inline static std::vector<std::string> posNames = {
			"Custom", "Top left", "Top right", "Bottom left", "Bottom right"
		};
		inline static DISPLAY_POS mSelectedPos = DISPLAY_POS::BOTTOM_RIGHT;
		inline static float fPosX = 0.0f;
		inline static float fPosY = 0.0f;
		inline static size_t mInterval = 1000;
		inline static size_t mLastInterval = 0;
		inline static int mTotalRam = 0;
	};

public:
	struct m_HotKeys
	{
		inline static HotKeyData aimSkinChanger;
		inline static HotKeyData freeCam;
		inline static HotKeyData commandWindow;
		inline static HotKeyData fixVeh;
		inline static HotKeyData flipVeh;
		inline static HotKeyData freeCamTeleportPlayer{VK_RETURN,VK_RETURN};
		inline static HotKeyData godMode;
		inline static HotKeyData menuOpen;
		inline static HotKeyData quickSceenShot;
		inline static HotKeyData quickTeleport;
		inline static HotKeyData vehEngine;
		inline static HotKeyData vehInstantStart;
		inline static HotKeyData vehInstantStop;
	};

	struct m_Commands
	{
		inline static bool m_bShowMenu = false;
		inline static char m_nInputBuffer[INPUT_BUFFER_SIZE] = "";
	};

	Menu();
	static void Draw();
	static void DrawOverlay();
	static void DrawShortcutsWindow();
	static void ProcessCommands();
};
