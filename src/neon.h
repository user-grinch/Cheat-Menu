#pragma once
#include "extender/VehicleExtender.h"

class Neon
{
private:
	class NeonData
	{
	public:
		CRGBA m_Color;
		bool m_bNeonInstalled;
		float m_fVal;
		uint m_nTimer;
		bool m_bIncrement;
		bool m_bPulsing;

		NeonData(CVehicle* pVeh)
		{
			m_bNeonInstalled = false;
			m_fVal = 0.0;
			m_nTimer = 0;
			m_bIncrement = true;
		}
	};

	static inline RwTexture* m_pNeonTexture = nullptr; // pointer to the neon mask texture
	static inline VehicleExtendedData<NeonData> m_VehNeon;

public:
	Neon();
	~Neon();
	static void InstallNeon(CVehicle* veh, int red, int green, int blue);
	static bool IsNeonInstalled(CVehicle* veh);
	static bool IsPulsingEnabled(CVehicle* veh);
	static void SetPulsing(CVehicle* veh, bool state);
	static void RemoveNeon(CVehicle* veh);
};
