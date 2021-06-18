#pragma once
#include "Player.h"
#include "Weapon.h"

class Ped
{
private:
	inline static SSearchData m_PedData{"ped"};
	inline static CJson m_SpecialPedJson = CJson("ped special");
	inline static bool m_bImagesLoaded;
	inline static bool m_bExGangWarsInstalled;
	inline static int m_nPedRemoveRadius = 5;
	static struct
	{
		inline static std::vector<CPed*> m_List;
		inline static int m_nAccuracy = 50;
		inline static int m_nPedHealth = 100;
		inline static bool m_bPedMove;
		inline static bool m_bPedBleed;
		inline static int m_nSelectedPedType;
		inline static int m_nWeaponId;
		inline static std::vector<std::string> m_PedTypeList =
		{
			"Civ male", "Civ female", "Cop", "Ballas", "Grove Street Families", "Los Santos Vagos",
			"San Fierro Rifa", "Da Nang Boys", "Mafia", "Mountain Cloud Triads", "Varrio Los Aztecas",
			"Gang 9", "Medic", "Dealer", "Criminal", "Fireman", "Prostitute"
		};
	} m_SpawnPed;
	inline static std::vector<std::string> m_GangNames =
	{
		"Ballas", "Grove street families", "Los santos vagos", "San fierro rifa",
		"Da nang boys", "Mafia", "Mountain cloud triad", "Varrio los aztecas", "Gang9", "Gang10"
	};

	friend class Player;
	friend class Weapon;
public:
	Ped();
	~Ped();
	static void Draw();
	static void SpawnPed(std::string& model);
};
