#pragma once
#include "pch.h"
#include "interface/ifeature.hpp"

/*
*   Main CheatMenu Class
*   Handles rendering, resizing, page drawing etc.
*/
class CheatMenuMgr : public IFeature<CheatMenuMgr>
{
private:
    ImVec2 m_fSize;
    bool m_bVisible;      // should the menu be drawn
    bool m_bSizeUpdated;  // Was menu size change requested
    bool m_bIsOnline;     // SAMP & VCMP flag

    friend class IFeature;
    CheatMenuMgr();
    CheatMenuMgr(const CheatMenuMgr&);

    // Applies imgui theme to the menu
    void ApplyStyle();

    // Draws the window ui each frame
    void Draw();

public:
    // Returns true if the menu is being shown
    bool IsVisible();

    // Resets the menu height & width to default
    void ResetSize();
};

extern CheatMenuMgr& CheatMenu;
