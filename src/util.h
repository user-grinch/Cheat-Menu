#pragma once
#include "tchar.h"
#include "pdh.h"

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
    static void ClearCharTasksVehCheck(CPed* ped);
    static int GetLargestGangInZone();
#endif

    static CPed* GetClosestPed();
    static CVehicle* GetClosestVehicle();
    static void GetCPUUsageInit();
    static double GetCurrentCPUUsage();
    static std::string GetLocationName(CVector* pos);
    static bool IsOnCutscene();
    static bool IsOnMission();
    static void RainbowValues(int& r, int& g, int& b, float speed);
};
