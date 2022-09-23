#pragma once
#include <extender/VehicleExtender.h>
#include "interface/icheat.hpp"
/*
	Vehicle Neon implementation class for GTA: San Andreas
	Handles neon colors and neon color changing

	TODO: Implement for VC & 3 too (maybe)
	Dunno how it'd work with the d3d8to9 wrapper
*/
class NeonMgr : public ICheat<NeonMgr>
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

    RwTexture* m_pNeonTexture = nullptr; // pointer to the neon mask texture
    VehicleExtendedData<NeonData> m_VehNeon;

    friend class IFeature;
    NeonMgr();
    NeonMgr(NeonMgr&);
    ~NeonMgr();

public:

    // Returns internal neon data
    NeonData GetData(CVehicle *pVeh);

    // Installs neons with color
    void Install(CVehicle* veh, int r, int g, int b);

    // Is neon installer for particular vehicle
    bool IsInstalled(CVehicle* veh);

    // Is neon pulsing enabled for vehicle
    bool IsPulsingEnabled(CVehicle* veh);

    // Set neon pulsing state
    void SetPulsing(CVehicle* veh, bool state);

    // Removes neon from vehicle
    void Remove(CVehicle* veh);
};

extern NeonMgr& Neon;