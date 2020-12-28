#pragma once
#include "Ped.h"
class Weapon
{
private:
	static ImGuiTextFilter filter;
	static std::string selected_item;
	static std::vector<std::string> search_categories;
	static std::vector<std::unique_ptr<TextureStructure>> weapon_vec;

	static CJson weapon_json;
	
	static bool auto_aim;
	static bool fast_reload;
	static bool huge_damage;
	static bool long_range;
	static bool rapid_fire;
	static bool dual_weild;
	static bool move_aim;
	static bool move_fire;

	static int ammo_count;
	static uchar cur_weapon_slot;

	static int selected_gang;
	static int selected_weapon_count;
	static int cur_weapon;
	static int gang_weapons[10][3];

	friend class Ped;
public:
	Weapon();
	~Weapon();

	static void Main();
	static void GiveWeaponToPlayer(std::string& weapon_type);
	static void SetGangWeapon(std::string& weapon_type);
};

