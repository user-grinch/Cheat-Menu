#pragma once

class NeonAPI
{
private:
	static bool api_installed;
	static HMODULE hapi;
public:
	NeonAPI();
	~NeonAPI();
	static bool InstallNeon(CVehicle *veh, int red, int green, int blue);
	static bool IsNeonInstalled(CVehicle *veh);
	static bool RemoveNeon(CVehicle *veh);
};

