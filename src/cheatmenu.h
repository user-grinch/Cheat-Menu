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
    static inline bool m_bShowMenu = false;                     // should the menu be drawn
    static inline ImVec2 m_fMenuSize = ImVec2(screen::GetScreenWidth() / 4, screen::GetScreenHeight() / 1.2);
    static inline bool m_bSizeChangedExternal = false;          // Was menu size change requested
    static inline std::vector<HeaderData> m_headerList;

    // Applies imgui theme to the menu
    static void ApplyStyle();

    // Draws the window ui each frame
    // Also handles drawing info, overlay, command window
    static void DrawWindow();

    static void ShowAnniversaryPage();
    static void ShowUpdatePage();
    static void ShowWelcomePage();

    // Does all the processing required to handle menu pages
    static void ProcessPages();

public:
    CheatMenu() = delete;
    CheatMenu(const CheatMenu&) = delete;

    // Initilizes the menu, page, hooks etc
    static void Init();

    // Returns true if the menu is being shown
    static bool IsMenuShown();

    // Resets the menu height & width to default
    static void ResetMenuSize();

    // Generates menu headers
    // Needs to be called after language change or adding new headers
    static void GenHeaderList();
};
