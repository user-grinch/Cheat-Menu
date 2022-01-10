#pragma once
#include "extender/VehicleExtender.h"

/*
	Vehicle Neon implementation class for GTA: San Andreas
	Handles neon colors and neon color changing

	TODO: Implement for VC & 3 too (maybe)
	Dunno how it'd work with the d3d8to9 wrapper
*/
class Neon
{
private:
    class NeonData
    {
    public:
        CRGBA m_Color;
        bool m_bNeonInstalled;
        float m_fVal;
        bool m_bIncrement;
        bool m_bPulsing;

        NeonData(CVehicle* pVeh)
        {
            m_bNeonInstalled = false;
            m_fVal = 0.0;
            m_bIncrement = true;
        }
    };

    static inline RwTexture* m_pNeonTexture = nullptr; // pointer to the neon mask texture
    static inline VehicleExtendedData<NeonData> m_VehNeon;

public:
    Neon() = delete;
    Neon(Neon&) = delete;

    static void InjectHooks();
    static void Install(CVehicle* veh, int red, int green, int blue);
    static bool IsInstalled(CVehicle* veh);
    static bool IsPulsingEnabled(CVehicle* veh);
    static void SetPulsing(CVehicle* veh, bool state);
    static void RemoveHooks();
    static void Remove(CVehicle* veh);
};
