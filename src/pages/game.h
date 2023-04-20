#pragma once
#include "pch.h"
#include "interface/ipage.h"

class GamePage : public IPage<GamePage>
{
private:
    ResourceStore m_MissionData{ "missions", eResourceType::TYPE_TEXT };
    bool m_bDisableCheats;
    bool m_bDisableReplay;
    bool m_bMissionTimer;
    bool m_bMobileRadio;

    struct
    {
        bool m_bEnabled;
        float m_fBacHealth;
        float m_fBacMaxHealth;
        float m_fBacArmour;
        float m_fBacStamina;
    } m_HardMode;

#ifdef GTASA
    bool m_bForbiddenArea = true;   // Wanted level when going outside playable aea
    bool m_bSolidWater;             // Walk on water hack
    bool m_bNoWaterPhysics;
    bool m_bScreenShot;
    ResourceStore m_StatData{ "stats", eResourceType::TYPE_TEXT };
#endif

    friend class IFeature;
    GamePage();
    GamePage(const GamePage&);

public:
    bool m_bFreezeTime;
    bool m_bSyncTime;               // Sync time with system

#ifdef GTASA
    int m_nCameraZoom = 70.0f;
    bool m_bLockCameraZoom;
#endif

    void Draw();
};

extern GamePage& gamePage;