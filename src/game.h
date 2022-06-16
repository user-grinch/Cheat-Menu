#pragma once
#include "pch.h"

class Game
{
private:
    static inline ResourceStore m_MissionData{ "missions", eResourceType::TYPE_TEXT };
    static inline bool m_bDisableCheats;
    static inline bool m_bDisableReplay;
    static inline bool m_bMissionTimer;

    struct m_Freecam
    {
        static inline bool m_bEnabled;
        static inline int m_nMul = 1;
        static inline float m_fFOV = 60.0f;
        static inline bool m_bInitDone;
        static inline CPed* m_pPed;
        static inline int m_nPed = -1;
        static inline CVector m_fMouse;
        static inline CVector m_fTotalMouse;
        static inline BYTE m_bHudState;
        static inline BYTE m_bRadarState;
    };
    struct m_HardMode
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
    static inline bool m_bScreenShot;
    static inline bool m_bKeepStuff;
    static inline ResourceStore m_StatData{ "stats", eResourceType::TYPE_TEXT };

    static inline std::vector<std::string> m_DayNames =
    {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

    struct m_RandomCheats
    {
        static inline bool m_bEnabled;
        static inline bool m_bProgressBar = true;
        static inline std::string m_EnabledCheats[92][2];
        static inline int m_nInterval = 10;
        static inline DataStore m_pData {"cheats"};
        static inline uint m_nTimer;
    };
#endif

    static void RealTimeClock();
    static void FreeCam();
    static void ClearFreecamStuff();

public:
    static inline bool m_bFreezeTime;
    static inline bool m_bSyncTime;

    Game() = delete;
    Game(const Game&) = delete;

    static void Init();
    static void ShowPage();
};
