#pragma once
#include "pch.h"

class Menu
{
private:

    enum DisplayPos
    {
        CUSTOM,
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };

    struct m_Overlay
    {
        static inline bool bCoord;
        static inline bool bFPS;
        static inline int mFPS;
        static inline bool bLocName;
        static inline bool bTransparent;
        static inline bool bVehHealth;
        static inline bool bVehSpeed;
        static inline bool bCpuUsage;
        static inline float fCpuUsage;
        static inline bool bMemUsage;
        static inline float fMemUsage;
        static inline std::vector<std::string> posNames =
        {
            "Custom", "Top left", "Top right", "Bottom left", "Bottom right"
        };
        static inline DisplayPos mSelectedPos = DisplayPos::BOTTOM_RIGHT;
        static inline float fPosX;
        static inline float fPosY;
        static inline int mTotalRam = 0;
        static inline float textColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    };  

public:
    struct m_Commands
    {
        static inline bool m_bShowMenu;
        static inline char m_nInputBuffer[INPUT_BUFFER_SIZE] = "";
    };

    Menu() = delete;
    Menu(const Menu&) = delete;

    static void Init();
    static void ShowPage();
    static void DrawOverlay();
    static void DrawShortcutsWindow();
    static void ProcessCommands();
};
