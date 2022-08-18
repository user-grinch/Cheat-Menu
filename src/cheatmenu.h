#pragma once
#include "pch.h"

/*
*   Main CheatMenu Class
*   Handles rendering, resizing, page drawing etc.
*/
class CheatMenu
{
private:
    static inline ImVec2 m_fMenuSize = ImVec2(screen::GetScreenWidth() / 4, screen::GetScreenHeight() / 1.2);
    static inline bool m_bShowMenu = false;             // should the menu be drawn
    static inline bool m_bSizeChangedExternal = false;  // Was menu size change requested

    // Applies imgui theme to the menu
    static void ApplyStyle();

    // Draws the window ui each frame
    static void DrawWindow();

public:
    CheatMenu() = delete;
    CheatMenu(const CheatMenu&) = delete;

    // Initilizes the menu, page, hooks etc
    static void Init();

    // Returns true if the menu is being shown
    static bool IsBeingDrawn();

    // Resets the menu height & width to default
    static void ResetSize();
};
