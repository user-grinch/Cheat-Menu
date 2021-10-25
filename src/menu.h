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
		static inline bool bCoord = false;
		static inline bool bFPS = false;
		static inline int mFPS = 0;
		static inline bool bLocName = false;
		static inline bool bTransparent = false;
		static inline bool bVehHealth = false;
		static inline bool bVehSpeed = false;
		static inline bool bCpuUsage = false;
		static inline float fCpuUsage = 0.0f;
		static inline bool bMemUsage = false;
		static inline float fMemUsage = 0.0f;
		static inline std::vector<std::string> posNames = {
			"Custom", "Top left", "Top right", "Bottom left", "Bottom right"
		};
		static inline DISPLAY_POS mSelectedPos = DISPLAY_POS::BOTTOM_RIGHT;
		static inline float fPosX = 0.0f;
		static inline float fPosY = 0.0f;
		static inline size_t mInterval = 1000;
		static inline size_t mLastInterval = 0;
		static inline int mTotalRam = 0;
	};

public:
	struct m_Commands
	{
		static inline bool m_bShowMenu = false;
		static inline char m_nInputBuffer[INPUT_BUFFER_SIZE] = "";
	};

	Menu();
	static void Draw();
	static void DrawOverlay();
	static void DrawShortcutsWindow();
	static void ProcessCommands();
};
