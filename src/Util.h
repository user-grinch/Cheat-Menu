#pragma once
#include "tchar.h"
#include "pdh.h"

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

class Util
{
public:
	static void ClearCharTasksVehCheck(CPed* ped);
	static CPed* GetClosestPed();
	static CVehicle* GetClosestVehicle();
	static int GetLargestGangInZone();
	static bool IsOnCutscene();
	static bool IsOnMission();
	static std::string GetLocationName(CVector* pos);
	static void RainbowValues(int& r, int& g, int& b, float speed);
	static void GetCPUUsageInit();
	static double GetCurrentCPUUsage();
	static void* GetTextureFromRaster(RwTexture* pTexture);
	static void LoadTextureDirectory(SSearchData& data, char* path, bool pass_full_name = false);
	static RwTexture* LoadTextureFromMemory(char* data, unsigned int size);
};
