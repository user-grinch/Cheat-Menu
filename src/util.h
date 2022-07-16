#pragma once
#include "tchar.h"
#include "pdh.h"

/*
*   Contains utility functions 
*/
class Util
{
private:
    static inline PDH_HQUERY cpuQuery;
    static inline PDH_HCOUNTER cpuTotal;
    static inline ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    static inline int numProcessors;
    static inline HANDLE self;

public:
    Util() = delete;
    Util(Util&) = delete;

#ifdef GTASA
    static int GetLargestGangInZone();
    static void ClearCharTasksVehCheck(CPed* ped);
    static bool IsOnMission();
#endif

    static void UnFlipVehicle(CVehicle *pVeh);
    static void FixVehicle(CVehicle *pVeh);
    static CPed* GetClosestPed();
    static CVehicle* GetClosestVehicle();
    static void GetCPUUsageInit();
    static double GetCurrentCPUUsage();
    static std::string GetLocationName(CVector* pos);
    static bool IsOnCutscene();
    static bool IsInVehicle(CPed *pPed = FindPlayerPed());
    static void RainbowValues(int& r, int& g, int& b, float speed);
    static void SetCarForwardSpeed(CVehicle *pVeh, float speed);
    static float RoundFloat(float val);
};
