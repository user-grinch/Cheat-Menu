#pragma once

class Weapon
{
public:
	inline static ImGuiTextFilter filter = "";
	inline static std::string selected_item = "All";
	inline static std::vector<std::string> search_categories;
	inline static std::vector<std::unique_ptr<TextureStructure>> weapon_vec;
	inline static bool images_loaded = false;
	
	inline static CJson weapon_json = CJson("weapon");

	inline static bool auto_aim = false;
	inline static bool fast_reload = false;
	inline static bool huge_damage = false;
	inline static bool long_range = false;
	inline static bool rapid_fire = false;
	inline static bool dual_weild = false;
	inline static bool move_aim = false;
	inline static bool move_fire = false;

	inline static int ammo_count = 99999;
	inline static uchar cur_weapon_slot = -1;

	inline static int selected_gang = 0;
	inline static int selected_weapon_count = 0;
	inline static int gang_weapons[10][3] = 
	{
		{WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED },		// Ballas
		{WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED},		// Grove
		{WEAPON_PISTOL, WEAPON_UNARMED, WEAPON_UNARMED},		// Vagos
		{WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},		// SF Rifa
		{WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED},		// Da Nang Boys
		{WEAPON_DESERT_EAGLE , WEAPON_UNARMED, WEAPON_UNARMED},	// Mafia
		{WEAPON_PISTOL, WEAPON_AK47, WEAPON_UNARMED},			// Triads
		{WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_UNARMED},		// VLA
		{WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},		// Gang 9
		{WEAPON_UNARMED, WEAPON_UNARMED, WEAPON_UNARMED},		// Gang 10
	};

	Weapon();
	~Weapon();

	static void Draw();
	static void GiveWeaponToPlayer(std::string& weapon_type);
	static void SetGangWeapon(std::string& weapon_type);
};

