#pragma once

class Neon
{
private:
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
	
	inline static RwTexture* neon_texture = nullptr;
	inline static VehExtender<NeonData> VehNeon;

public:
	Neon();
	~Neon();
	static void InstallNeon(CVehicle* veh, int red, int green, int blue);
	static bool IsNeonInstalled(CVehicle* veh);
	static bool IsPulsingEnabled(CVehicle* veh);
	static void SetPulsing(CVehicle* veh, bool state);
	static void RemoveNeon(CVehicle* veh);
	static void RenderEvent(CVehicle* veh);
};

