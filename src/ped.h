#pragma once
#include "pch.h"

class Ped
{
private:
    static inline bool m_bBigHead;
    static inline bool m_bThinBody;

    struct Spawner
    {
        static inline std::vector<CPed*> m_List;
        static inline int m_nAccuracy = 50;
        static inline int m_nPedHealth = 100;
        static inline bool m_bPedMove;
        static inline bool m_bPedBleed;
        static inline int m_nSelectedPedType;
        static inline int m_nWeaponId;
    };

#ifdef GTASA
    static void SpawnPed(std::string& model);
#else
    static void SpawnPed(std::string& cat, std::string& name, std::string& model);
#endif

public:
#ifdef GTASA
    static inline DataStore m_SpecialPedData {"special_peds"};
    static inline ResourceStore m_PedData{"peds", eResourceType::TYPE_BOTH, ImVec2(65, 110)};
#else
    static inline ResourceStore m_PedData {"peds", eResourceType::TYPE_TEXT};
#endif

    Ped() = delete;
    Ped(const Ped&) = delete;

    static void Init();
    static void ShowPage();
};
