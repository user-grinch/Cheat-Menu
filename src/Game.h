#pragma once
#include "pch.h"
class Game
{
public:
	static inline ResourceStore m_MissionData{ "mission", eResourceType::TYPE_TEXT };

#ifdef GTASA
	static inline bool m_bForbiddenArea = true;
	static inline bool m_bSolidWater;
	static inline bool m_bScreenShot;
	static inline uint m_nSolidWaterObj;
	static inline bool m_bKeepStuff;
	static inline ResourceStore m_StatData{ "stat", eResourceType::TYPE_TEXT };
	
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
		static inline CJson m_Json = CJson("cheat name");
		static inline uint m_nTimer;
	};
	struct m_Freecam
	{
		static inline bool m_bEnabled;
		static inline float m_fSpeed = 0.08f;
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
	
#endif
	static inline bool m_bDisableCheats;
	static inline bool m_bDisableReplay;
	static inline bool m_bMissionTimer;
	static inline bool m_bFreezeTime;
	static inline bool m_bSyncTime;
	static inline uint m_nSyncTimer;
	static inline bool m_bMissionLoaderWarningShown;

	Game();
	static void Draw();
	static void RealTimeClock();

#ifdef GTASA
	static void FreeCam();
	static void ClearFreecamStuff();
#endif
};
