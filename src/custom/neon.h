#pragma once
#ifdef GTASA
#include <extender/VehicleExtender.h>

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

    // Injects necessary hooks into the game
    static void Init();

    // Installs neons with color
    static void Install(CVehicle* veh, int r, int g, int b);

    // Is neon installer for particular vehicle
    static bool IsInstalled(CVehicle* veh);

    // Is neon pulsing enabled for vehicle
    static bool IsPulsingEnabled(CVehicle* veh);

    // Set neon pulsing state
    static void SetPulsing(CVehicle* veh, bool state);

    // Removes the neon game hooks
    static void Shutdown();

    // Removes neon from vehicle
    static void Remove(CVehicle* veh);
};
#endif