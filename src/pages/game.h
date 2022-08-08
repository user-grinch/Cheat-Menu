#pragma once
#include "pch.h"

class Game
{
private:
    static inline ResourceStore m_MissionData{ "missions", eResourceType::TYPE_TEXT };
    static inline bool m_bDisableCheats;
    static inline bool m_bDisableReplay;
    static inline bool m_bMissionTimer;
    static inline bool m_bMobileRadio;

    struct HardMode
    {
        static inline bool m_bEnabled;
        static inline float m_fBacHealth = 0.0f;
        static inline float m_fBacMaxHealth = 0.0f;
        static inline float m_fBacArmour = 0.0f;
        static inline float m_fBacStamina = 0.0f;
    };

#ifdef GTASA
    static inline bool m_bForbiddenArea = true; // wanted level when going outside playable aea
    static inline bool m_bSolidWater; // walk on water hack
    static inline bool m_bNoWaterPhysics;
    static inline bool m_bScreenShot;
    static inline bool m_bKeepStuff;
    static inline ResourceStore m_StatData{ "stats", eResourceType::TYPE_TEXT };
#endif

public:
    static inline bool m_bFreezeTime;
    static inline bool m_bSyncTime;

#ifdef GTASA
    static inline int m_nCameraZoom = 70.0f;
    static inline bool m_bLockCameraZoom;
#endif 

    Game() = delete;
    Game(const Game&) = delete;

    static void Init();
    static void ShowPage();
};
