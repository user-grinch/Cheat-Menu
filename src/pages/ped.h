#pragma once
#include "pch.h"
#include "interface/ipage.h"

class PedPage : public IPage<PedPage>
{
private:
    bool m_bBigHead;
    bool m_bThinBody;
    struct 
    {
        std::vector<CPed*> m_List;
        int m_nAccuracy = 50;
        int m_nPedHealth = 100;
        bool m_bPedMove;
        bool m_bPedBleed;
        int m_nSelectedPedType;
        int m_nWeaponId;
    } m_Spawner;


    friend class IFeature;
    PedPage();
    PedPage(const PedPage&);

    // Add new ped to data file
    void AddNewPed();

    // Spawn a ped with specified model
#ifdef GTASA
    void SpawnPed(str& model);
#else
    void SpawnPed(str& cat, str& name, str& model);
#endif
    
public:
#ifdef GTASA
    DataStore m_SpecialPedData {"special_peds"};
    ResourceStore m_PedData{"peds", eResourceType::TYPE_IMAGE_TEXT, ImVec2(65, 110)};
#else
    ResourceStore m_PedData {"peds", eResourceType::TYPE_TEXT};
#endif

    void Draw();
};

extern PedPage &pedPage;
