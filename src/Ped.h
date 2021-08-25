#pragma once
#include "pch.h"
#include "Player.h"
#ifdef GTASA
#include "Weapon.h"
#endif
class Ped
{
private:
#ifdef GTASA
	inline static bool m_bBigHead;
	inline static bool m_bThinBody;
	inline static CJson m_SpecialPedJson = CJson("ped special");
	inline static ResourceStore m_PedData{"ped", eResourceType::TYPE_BOTH, ImVec2(65, 110)};
#elif GTAVC
	inline static ResourceStore m_PedData{"ped", eResourceType::TYPE_TEXT};
#endif
	inline static bool m_bImagesLoaded;
	inline static bool m_bExGangWarsInstalled;
	inline static int m_nPedRemoveRadius = 5;
	struct m_SpawnPed
	{
		inline static std::vector<CPed*> m_List;
		inline static int m_nAccuracy = 50;
		inline static int m_nPedHealth = 100;
		inline static bool m_bPedMove;
		inline static bool m_bPedBleed;
		inline static int m_nSelectedPedType;
		inline static int m_nWeaponId;
		inline static std::string m_nWeaponName = "None";
		inline static std::vector<std::string> m_PedTypeList =
		{
#ifdef GTASA
			"Civ Male", "Civ Female", "Cop", "Ballas", "Grove Street Families", "Los Santos Vagos",
			"San Fierro Rifa", "Da Nang Boys", "Mafia", "Mountain Cloud Triads", "Varrio Los Aztecas",
			"Gang 9", "Medic", "Dealer", "Criminal", "Fireman", "Prostitute"
#elif GTAVC
			"Civ Male", "Civ Female", "Cop (crash)", "Cubans", "Haitians", "Streetwannabe's", "Diaz' Gang",
			"Security Guards", "Biker Gang", "Vercetti Gang", "Golfers", "Gang 9", "Emergency",
			"Fireman", "Criminal", "Unused", "Prostitute", "Special"
#endif
		};
	};
	inline static std::vector<std::string> m_GangNames =
	{
		"Ballas", "Grove street families", "Los santos vagos", "San fierro rifa",
		"Da nang boys", "Mafia", "Mountain cloud triad", "Varrio los aztecas", "Gang9", "Gang10"
	};

	friend class Player;
#ifdef GTASA
	friend class Weapon;
#endif
public:
	Ped();
	~Ped();
	static void Draw();

#ifdef GTASA
	static void SpawnPed(std::string& model);
	static void BigHeadEffect(CPed *ped);
#elif GTAVC
	static void SpawnPed(std::string& cat, std::string& name, std::string& model);
#endif
};
