#pragma once
#include "pch.h"
#include "player.h"
#ifdef GTASA
#include "weapon.h"
#endif

class Ped
{
private:
#ifdef GTASA
	static inline bool m_bBigHead;
	static inline bool m_bThinBody;
	static inline CJson m_SpecialPedJson = CJson("ped special");
	static inline ResourceStore m_PedData{"ped", eResourceType::TYPE_BOTH, ImVec2(65, 110)};
#elif GTAVC
	static inline ResourceStore m_PedData{"ped", eResourceType::TYPE_TEXT};
#endif
	static inline bool m_bImagesLoaded;
	static inline bool m_bExGangWarsInstalled;
	static inline int m_nPedRemoveRadius = 5;
	struct m_SpawnPed
	{
		static inline std::vector<CPed*> m_List;
		static inline int m_nAccuracy = 50;
		static inline int m_nPedHealth = 100;
		static inline bool m_bPedMove;
		static inline bool m_bPedBleed;
		static inline int m_nSelectedPedType;
		static inline int m_nWeaponId;
		static inline std::string m_nWeaponName = "None";
		static inline std::vector<std::string> m_PedTypeList =
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
	static inline std::vector<std::string> m_GangNames =
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
