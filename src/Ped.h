#pragma once
#include "Player.h"
#include "Weapon.h"

class Ped
{
private:
	static ImGuiTextFilter filter;
	static std::string selected_item;
	static std::vector<std::string> search_categories;
	static std::vector<std::unique_ptr<TextureStructure>> peds_vec;

	static CJson ped_json;
	static CJson pedspecial_json;

	static std::vector<std::string> gang_names;
	static bool exgangwars_installed;


	struct spawn_ped {
		static std::vector<CPed*> list;
		static int accuracy;
		static int health;
		static bool dont_move;
		static bool ped_bleed;
		static std::vector<std::string> ped_type;
		static int selected_ped_type;
		static int weapon_id;
	};

	friend class Player;
	friend class Weapon;
public:
	Ped();
	~Ped();
	static void Main();
	static void SpawnPed(std::string& model);
};

