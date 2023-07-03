#pragma once
#include "pch.h"
#include "interface/ipage.h"

class VehiclePage : public IPage<VehiclePage>
{
private:
    bool m_bAutoUnflip;
    bool m_bBikeFly;
    bool m_bDontFallBike;
    bool m_bVehHeavy;
    bool m_bVehWatertight;
    bool m_bNoDamage;
    std::string m_DoorNames[6] =
    { "Hood", "Boot", "Front left door", "Front right door", "Rear left door", "Rear right door" };
    bool m_bLockSpeed;
    float m_fLockSpeed;

#ifdef GTASA
    bool m_bDisableColDetection;
    bool m_bVehFlipNoBurn;
    bool m_bNoDerail;
#endif

#ifndef GTA3
    int m_nVehicleVariant = 0;
#endif

    struct
    {
        ResourceStore m_VehData { "vehicles", BY_GAME(eResourceType::TYPE_IMAGE_TEXT, eResourceType::TYPE_TEXT,
                    eResourceType::TYPE_TEXT), ImVec2(100, 75)};
        bool m_bAsDriver = true;
        bool m_bInAir = true;
        bool m_bWithTunes = true;
        char m_nLicenseText[9];
    } m_Spawner;

    friend class IFeature;
    VehiclePage();
    VehiclePage(const VehiclePage&);

public:
    void AddNew();
    void Draw();

#ifdef GTASA
    int GetRandomTrainIdForModel(int model);
    void SpawnVehicle(std::string& name);
#else
    void SpawnVehicle(std::string& rootkey, std::string& vehName, std::string& model);
#endif
};

extern VehiclePage& vehiclePage;