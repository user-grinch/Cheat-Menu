#pragma once
#include "CWeapon.h"

class Weapon
{
public:
#ifdef GTASA
	static inline ResourceStore m_WeaponData{ "weapon", eResourceType::TYPE_BOTH, ImVec2(65, 65) };

	static inline bool m_bAutoAim;
	static inline bool m_bRapidFire;
	static inline bool m_bDualWeild;
	static inline bool m_bMoveAim;
	static inline bool m_bMoveFire;
	static inline int m_nSelectedGang;
#else // GTA3 & GTAVC
	static inline ResourceStore m_WeaponData{ "weapon", eResourceType::TYPE_TEXT };
	static inline bool m_bInfiniteAmmo;
#endif
	static inline bool m_bFastReload;
	static inline bool m_bHugeDamage;
	static inline bool m_bLongRange;
	static inline int m_nAmmoCount = 99999;
	static inline uchar m_nCurrentWeaponSlot = -1;
	static inline int m_nSelectedWeapon;
	
#ifdef GTASA
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
#endif

	Weapon();

	static void Draw();

#ifdef GTASA
	static void GiveWeaponToPlayer(std::string& weapon_type);
#else // GTA3 & GTAVC
	static void GiveWeaponToPlayer(std::string& rootkey, std::string& model, std::string& name);
#endif
	
#ifdef GTASA
	static void SetGangWeapon(std::string& weapon_type);
#endif
};
