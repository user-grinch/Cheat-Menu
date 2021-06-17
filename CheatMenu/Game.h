#pragma once
class Game
{
public:
	inline static SearchData mission_data{ "mission" };
	inline static std::vector<std::string> day_names = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };

	inline static char save_game_name[22] = "";

	struct random_cheats
	{
		inline static bool enable = false;
		inline static int enable_wait_time = 10;
		inline static CJson name_json =  CJson("cheat name");
		inline static uint timer = 0;
		inline static std::string enabled_cheats[92][2];
	};

	struct freecam
	{
		inline static bool enable = false;
		inline static float speed = 0.08f;
		inline static float fov = -1;
		inline static bool init_done = false;
		inline static CPed* ped = nullptr;
		inline static int hped = -1;
		inline static float mouseX = 0.0f; 
		inline static float mouseY = 0.0f;
		inline static float tmouseX = 0.0f;
		inline static float tmouseY = 0.0f;
	};

	struct hard_mode
	{
		inline static bool state = false;
		inline static float prev_health = 0.0f;
		inline static float prev_max_health = 0.0f;
		inline static float prev_armour = 0.0f;
		inline static float prev_stamina = 0.0f;
	};

	inline static bool disable_cheats = false;
	inline static bool disable_replay = false;
	inline static bool forbidden_area_wl = true;
	inline static bool freeze_mission_timer = false;
	inline static bool freeze_time = false;
	inline static bool keep_stuff = false;
	inline static bool solid_water = false;
	inline static bool ss_shortcut = false;
	inline static bool sync_time = false;
	inline static uint sync_time_timer = 0;
	inline static uint solid_water_object = 0;
	inline static bool mission_warning_shown = false;

	struct stat
	{
		inline static CJson json = CJson("stat");
		inline static ImGuiTextFilter filter = "";
		inline static std::vector<std::string> search_categories;
		inline static std::string selected_item = "All";
	};

	Game();
	static void Draw();
	static void FreeCam();
	static void ClearFreecamStuff();
	static void RealTimeClock();
};

