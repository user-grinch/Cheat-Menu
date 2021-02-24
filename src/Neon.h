#pragma once

class Neon
{
private:
	static RwTexture* neon_texture;

	class NeonData {
	public:
		CRGBA color;
		bool neon_installed;
		float val;
		uint timer;
		bool increment;
		bool pulsing;

		NeonData(CVehicle* pVeh)
		{
			neon_installed = false;
			val = 0.0;
			timer = 0;
			increment = true;
		}
	};

	static VehicleExtendedData<NeonData> VehNeon;

public:
	Neon();
	~Neon();
	static void InstallNeon(CVehicle* veh, int red, int green, int blue);
	static bool IsNeonInstalled(CVehicle* veh);
	static bool IsPulsingEnabled(CVehicle* veh);
	static void SetPulsing(CVehicle* veh, bool state);
	static void RemoveNeon(CVehicle* veh);
};

