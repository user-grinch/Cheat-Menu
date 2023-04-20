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

    struct GangStruct
    {
        int m_nSelected;
        int m_nSelectedMember;

        unsigned int GetModel(unsigned int gangId, unsigned int memberId);
        void SetModel(unsigned int gangId, unsigned int memberId, unsigned int model);
        void ResetModels();
    } m_Gang;


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

    const char* m_GangList =
    {
        "Ballas\0Grove street families\0Los santos vagos\0San fierro rifa\0Da nang boys\0"
        "Mafia\0Mountain cloud triad\0Varrio los aztecas\0Gang9\0Gang10\0"
    };
#else
    ResourceStore m_PedData {"peds", eResourceType::TYPE_TEXT};
#endif

    void Draw();
};

extern PedPage &pedPage;
