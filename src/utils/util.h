#pragma once
#include <tchar.h>
#include <pdh.h>

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
    static void ClearCharTasksCarCheck(CPed* ped);
    static int GetLargestGangInZone();
    static bool IsOnMission();
#endif

    static ImVec2 ConvertScreenToMap(ImVec2 pos, ImVec2 mapSz, ImVec2 screenSz);
    static ImVec2 ConvertMapToScreen(ImVec2 pos, ImVec2 mapSz, ImVec2 screenSz);

    static void FixCar(CVehicle *pVeh);
    static void SetCarForwardSpeed(CVehicle *pVeh, float speed);
    static CVehicle* GetClosestCar();
    static int GetCarModel(const char* name);
    static std::string GetCarName(int model);
    static bool IsInCar(CPed *pPed = FindPlayerPed());
    static void UnFlipCar(CVehicle *pVeh);

    static CPed* GetClosestPed();

    static void GetCPUUsageInit();
    static double GetCurrentCPUUsage();
    static float RoundFloat(float val);

    static std::string GetLocationName(CVector* pos);
    static bool IsOnCutscene();
    static void RainbowValues(int& r, int& g, int& b, float speed);
    static void SetMessage(const char *message, bool b1 = false, bool b2 = false, bool b3 = false);
};
