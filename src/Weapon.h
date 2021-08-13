#pragma once

class Weapon
{
public:
#ifdef GTASA
	inline static ResourceStore m_WeaponData{ "weapon", eResourceType::TYPE_BOTH, ImVec2(65, 65) };
#elif GTAVC
	inline static ResourceStore m_WeaponData{ "weapon", eResourceType::TYPE_TEXT };
#endif
	inline static bool m_bAutoAim;
	inline static bool m_bFastReload;
	inline static bool m_bHugeDamage;
	inline static bool m_bLongRange;
	inline static bool m_bRapidFire;
	inline static bool m_bDualWeild;
	inline static bool m_bMoveAim;
	inline static bool m_bMoveFire;
	inline static int m_nAmmoCount = 99999;
	inline static uchar m_nCurrentWeaponSlot = -1;
	inline static int m_nSelectedGang;
	inline static int m_nSelectedWeapon;

#ifdef GTASA
	inline static int m_nGangWeaponList[10][3] =
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
	static void GiveWeaponToPlayer(std::string& weapon_type);
	static void SetGangWeapon(std::string& weapon_type);
};
