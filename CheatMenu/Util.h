#pragma once
#include "TCHAR.h"
#include "pdh.h"

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

class Util
{
private:
	static bool LoadTextureFromFileDx11(const char* filename, ID3D11ShaderResourceView** out_srv);

public:
	static void ClearCharTasksVehCheck(CPed* ped);
	static CPed *GetClosestPed();
	static CVehicle *GetClosestVehicle();
	static int GetLargestGangInZone();
	static void LoadTexturesInDirRecursive(const char * path, const char * file_ext, std::vector<std::string>& category_vec, std::vector<std::unique_ptr<TextureStructure>> &store_vec);
	static void ReleaseTextures(std::vector<std::unique_ptr<TextureStructure>> &store_vec);
	static bool IsOnCutscene();
	static bool IsOnMission();
	static std::string GetLocationName(CVector *pos);
	static void RainbowValues(int &r, int&g, int &b, float speed);
	static RwTexture* LoadTextureFromPngFile(fs::path path);
	static void GetCPUUsageInit();
	static double GetCurrentCPUUsage();
};

