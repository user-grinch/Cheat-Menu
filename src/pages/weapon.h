#pragma once
#include "pch.h"
#include "interface/ipage.h"

class WeaponPage : public IPage<WeaponPage>
{
private:
    bool m_bFastReload;
    bool m_bHugeDamage;
    bool m_bLongRange;
    int m_nAmmoCount = 99999;

#ifdef GTASA
    bool m_bAutoAim;
    bool m_bRapidFire;
    bool m_bDualWeild;
    bool m_bMoveAim;
    bool m_bMoveFire;
    bool m_bNoSpread;
    struct GangStruct
    {
        int m_nSelected;
        int m_nSelectedWeapon;
        int m_WeaponList[10][3] =
        {
            {WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED},      // Ballas
            {WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED},        // Grove
            {WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED},        // Vagos
            {WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},       // SF Rifa
            {WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED},      // Da Nang Boys
            {WEAPON_DESERT_EAGLE, WEAPON_UNARMED, WEAPON_UNARMED},  // Mafia
            {WEAPON_PISTOL, WEAPON_AK47, WEAPON_UNARMED},           // Triads
            {WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED},      // VLA
            {WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},       // Gang 9
            {WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},       // Gang 10
        };

        void SetWeapon(std::string& weaponType);
    } m_Gang;

#else
    bool m_bInfiniteAmmo;
#endif

    friend class IFeature;
    WeaponPage();
    WeaponPage(const WeaponPage&);

public:
    ResourceStore m_WeaponData { "weapons", BY_GAME(eResourceType::TYPE_IMAGE_TEXT, eResourceType::TYPE_TEXT,
                eResourceType::TYPE_TEXT), ImVec2(65, 65) };

    // Add new weapon entry
    void AddNew();
    void Draw();

#ifdef GTASA
    void GiveWeaponToPlayer(std::string& weaponType);
#else
    void GiveWeaponToPlayer(std::string& rootkey, std::string& model, std::string& name);

    // Returns weapon model from weaponType
    int GetWeaponModel(eWeaponType weaponType);

    // Returns weaponType from model
    eWeaponType GetWeaponType(int weaponModel);
#endif
};

extern WeaponPage& weaponPage;

