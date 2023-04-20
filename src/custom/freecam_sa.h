#pragma once
#include "interface/icheat.hpp"

/*
    Freeroam Camera Mode for SA
    Similar to how airbreaks work
    But more flexible
*/
class FreecamMgr : public ICheat<FreecamMgr>
{
private:
    CPed* m_pPed;         // pointer to the dummy ped
    int m_nPed = -1;      // handle to the dummy ped
    bool m_bHudState;     // backup of the prev game hud state
    bool m_bRadarState;   // backup of the prev game radar state

    friend class IFeature;
    FreecamMgr();
    FreecamMgr(const FreecamMgr&);

    // Process freecam mode each frame
    void DrawPages();

public:
    int m_nMul = 1;       // speed multiplier
    float m_fFOV = 60.0f; // freecam field of view

    void Enable();
    void Disable();
};

extern FreecamMgr& Freecam;