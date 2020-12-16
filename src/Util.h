#pragma once
class Util
{
private:
	static bool LoadTextureFromFileDx11(const char* filename, ID3D11ShaderResourceView** out_srv);

public:
	static void ClearCharTasksVehCheck(CPed* ped);
	static CVehicle *GetClosestVehicle(CPlayerPed* player);
	static int GetLargestGangInZone();
	static void LoadTexturesInDirRecursive(const char * path, const char * file_ext, std::vector<std::string>& category_vec, std::vector<std::unique_ptr<TextureStructure>> &store_vec);
	static bool IsOnMission();
	static std::string GetLocationName(CVector *pos);
	static void RainbowValues(int &r, int&g, int &b, float speed);
};

