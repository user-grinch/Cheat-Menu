#pragma once
#include "pch.h"

/*
*   Main CheatMenu Class
*   Handles rendering, resizing, page handling etc.
*/
class CheatMenu
{
private:
    enum class eMenuPages
    {
        ANIMATION, ANNIVERSARY, GAME, MENU, NONE, PED, PLAYER, TELEPORT, UPDATE, VEHICLE, VISUAL, WEAPON, WELCOME
    };
    struct HeaderData
    {
        std::string name;
        void *pFunc;
        eMenuPages page;
        bool skipHeader = false;
    };
    
    static inline eMenuPages m_nMenuPage = eMenuPages::WELCOME; // current visible menu page
    static inline bool m_bShowMenu = false;
    static inline ImVec2 m_fMenuSize = ImVec2(screen::GetScreenWidth() / 4, screen::GetScreenHeight() / 1.2);
    static inline bool m_bSizeChangedExternal = false; // Was menu size change requested
    static inline std::vector<HeaderData> m_headerList;

    // Applies imgui theme to the menu
    static void ApplyStyle();

    // Draws the window ui each frame
    // Also handles drawing overlay & command window
    static void DrawWindow();

    static void ShowAnniversaryPage();
    static void ShowUpdatePage();
    static void ShowWelcomePage();

    // Does all the processing required to handle menu pages
    static void ProcessPages();

public:
    CheatMenu() = delete;
    CheatMenu(const CheatMenu&) = delete;

    static void Init();
    static bool IsMenuShown();
    static void ResetMenuSize();

    // Generates menu headers
    // Needs to be called after language change or adding new headers
    static void GenHeaderList();
};
