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
    ImVec2 m_fSize, m_fPos;
    bool m_bVisible;      // should the menu be drawn
    bool m_bWindowParamUpdated;  // Was menu size/pos changed
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
    void SetVisibility(bool flag);

    // Resets the menu height & width to default
    void ResetParams();
};

extern CheatMenuMgr& CheatMenu;
