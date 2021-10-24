#pragma once
#include "pdh.h"
#include "tchar.h"

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

class Util
{
public:
#ifdef GTASA
    static void ClearCharTasksVehCheck(CPed *ped);
    static int GetLargestGangInZone();
    static RwTexture *LoadTextureFromMemory(char *data, unsigned int size);
#endif

    static CPed *GetClosestPed();
    static CVehicle *GetClosestVehicle();
    static std::string GetLocationName(CVector *pos);
    static bool IsOnMission();
    static bool IsOnCutscene();
    static void RainbowValues(int &r, int &g, int &b, float speed);
    static void GetCPUUsageInit();
    static double GetCurrentCPUUsage();
};
