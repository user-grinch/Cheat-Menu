#pragma once
#include "Player.h"
#include "Weapon.h"

class Ped
{
private:
	inline static SearchData ped_data{ "ped" };
	inline static CJson pedspecial_json = CJson("ped special");
	inline static bool images_loaded = false;

	inline static std::vector<std::string> gang_names = 
	{
		"Ballas", "Grove street families", "Los santos vagos", "San fierro rifa",
		"Da nang boys", "Mafia", "Mountain cloud triad", "Varrio los aztecas", "Gang9", "Gang10" 
	};
	inline static bool exgangwars_installed = false;
	inline static int ped_remove_radius = 5;

	struct spawn_ped 
	{
		inline static std::vector<CPed*> list;
		inline static int accuracy = 50;
		inline static int health = 100;
		inline static bool dont_move = false;
		inline static bool ped_bleed = false;
		inline static std::vector<std::string> ped_type = 
		{ 
			"Civ male","Civ female","Cop","Ballas","Grove Street Families","Los Santos Vagos",
			"San Fierro Rifa","Da Nang Boys","Mafia","Mountain Cloud Triads","Varrio Los Aztecas",
			"Gang 9","Medic","Dealer","Criminal","Fireman","Prostitute" 
		};
		inline static int selected_ped_type;
		inline static int weapon_id = 0;
	};

	friend class Player;
	friend class Weapon;
public:
	Ped();
	~Ped();
	static void Draw();
	static void SpawnPed(std::string& model);
};

