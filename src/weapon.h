#pragma once
#include "pch.h"

class Weapon
{
private:
    static inline bool m_bFastReload;
    static inline bool m_bHugeDamage;
    static inline bool m_bLongRange;
    static inline int m_nAmmoCount = 99999;
    static inline int m_nSelectedWeapon;

#ifdef GTASA
    static inline bool m_bAutoAim;
    static inline bool m_bRapidFire;
    static inline bool m_bDualWeild;
    static inline bool m_bMoveAim;
    static inline bool m_bMoveFire;
    static inline bool m_bNoSpread;
    static inline int m_nSelectedGang;
    static inline int m_nGangWeaponList[10][3] =
    {
        {WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED}, // Ballas
        {WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED}, // Grove
        {WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED}, // Vagos
        {WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED}, // SF Rifa
        {WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED}, // Da Nang Boys
        {WEAPON_DESERT_EAGLE, WEAPON_UNARMED, WEAPON_UNARMED}, // Mafia
        {WEAPON_PISTOL, WEAPON_AK47, WEAPON_UNARMED}, // Triads
        {WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED}, // VLA
        {WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED}, // Gang 9
        {WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED}, // Gang 10
    };

#else
    static inline bool m_bInfiniteAmmo;
#endif

public:
    Weapon() = delete;
    Weapon(const Weapon&) = delete;

    static void Init();
    static void ShowPage();
    
    static void AddWeapon();
#ifdef GTASA
    static inline ResourceStore m_WeaponData { "weapons", eResourceType::TYPE_IMAGE_TEXT, ImVec2(65, 65) };
    
    static void GiveWeaponToPlayer(std::string& weapon_type);
    static void SetGangWeapon(std::string& weapon_type);
#else
    static inline ResourceStore m_WeaponData { "weapons", eResourceType::TYPE_TEXT };

    static void GiveWeaponToPlayer(std::string& rootkey, std::string& model, std::string& name);
#endif
};

