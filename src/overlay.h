#pragma once
#include <vector>
#include <string.h>

/*
	CheatMenu Overlay Class

    Handles drawing overlay windows on the screen
    This is seprate from the menu window
*/
class Overlay
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
    static inline std::vector<CEntity*> m_EntityList;
    static inline const float m_fMaxDistance = 50.0f;
    static inline ImVec2 m_fPos;
    static inline int m_nTotalMem = 0;

    // Draws model & it's name on screen (peds, objects, vehicles)
    static void ProcessModelInfo();

#ifdef GTASA
    // Processes current ped tasks & draws them ons creen
    static void ProcessPedTasks();
#endif

    // Processes commad bar & commands
    static void ProcessCmdBar();
    static void ProcessCommands(std::string&& str);

    // Displays fps, health, veh speed etc
    static void ProcessInfoBox();

public:
    static inline bool m_bCmdBar;
    static inline bool m_bCoord;
    static inline bool m_bCpuUsage;
    static inline bool m_bFPS;
    static inline int m_nFPS;
    static inline bool m_bLocName;
    static inline bool m_bMemUsage;
    static inline bool m_bModelInfo;
    static inline bool m_bPedTasks;
    static inline bool m_bTransparent;
    static inline bool m_bVehHealth;
    static inline bool m_bVehSpeed;
    static inline float m_fTextCol[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    static inline DisplayPos m_nSelectedPos = DisplayPos::BOTTOM_RIGHT;

    Overlay() = delete;
    Overlay(Overlay&) = delete;

    // Injects necessary hooks into the game
    static void Init();

    // Draws the actual overlay on screen
    static void Draw();
};
