#pragma once
class Game
{
public:
	static CJson json;
	static ImGuiTextFilter filter;
	static std::vector<std::string> search_categories;
	static std::string selected_item;
	static std::vector<std::string> day_names;

	static char save_game_name[22];

	struct random_cheats
	{
		static bool enable;
		static int enable_wait_time;
		static CJson name_json;
		static uint timer;
		static std::string enabled_cheats[92][2];
	};

	struct airbreak
	{
		static bool enable;
		static float speed;
		static bool init_done;
		static float tmouseX, tmouseY;
	};

	static bool disable_cheats;
	static bool disable_replay;
	static bool forbidden_area_wl;
	static bool freeze_mission_timer;
	static bool freeze_time;
	static bool keep_stuff;
	static bool solid_water;
	static bool ss_shortcut;
	static bool sync_time;

	static uint ss_shotcut_timer;
	static uint sync_time_timer;
	static uint solid_water_object;

	struct stat
	{
		static CJson json;
		static ImGuiTextFilter filter;
		static std::vector<std::string> search_categories;
		static std::string selected_item;
	};

	Game();
	~Game();
	static void Main();
	static void AirbreakMode(CPlayerPed* player, int hplayer);
	static void CameraMode();
	static void ClearAirbreakStuff();
};

