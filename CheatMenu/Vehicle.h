#pragma once
#include "Neon.h"
#include "Paint.h"

class Vehicle : Paint, Neon
{
private:
	inline static bool bike_fly = false;
	inline static bool dont_fall_bike = false;
	inline static bool veh_heavy = false;
	inline static bool veh_watertight = false;
	inline static bool veh_nodmg = false;
	inline static int door_menu_button = 0;
	inline static std::string door_names[6] = 
		{ "Hood","Boot","Front left door","Front right door","Rear left door","Rear right door" };
	inline static int veh_remove_radius = 0;

	inline static bool lock_speed = false;
	inline static float lock_speed_val = 0.0f;

	inline static std::map<int, std::string> vehicle_ide;
	inline static std::vector<std::vector<float>> carcols_color_values;
	inline static std::map<std::string, std::vector<int>> carcols_car_data;

	struct color
	{
		inline static bool material_filter = true;
		inline static int radio_btn = 1;
		inline static bool show_all = false;
		inline static float color_picker[3]{ 0,0,0 };
	};

	struct neon
	{
		inline static float color_picker[3]{ 0,0,0 };
		inline static bool rainbow = false;
		inline static uint rainbow_timer = 0;
		inline static bool traffic = false;
		inline static uint traffic_timer = 0;
	};

	struct spawner
	{
		inline static ImGuiTextFilter filter = "";
		inline static std::string selected_item = "All";
		inline static std::vector<std::string> search_categories;
		inline static std::vector<std::unique_ptr<TextureStructure>> image_vec;
		inline static bool spawn_inside = true;
		inline static bool spawn_in_air = true;
		inline static char license_text[9] = "";
	};

	struct texture9
	{
		inline static ImGuiTextFilter filter = "";
		inline static std::string selected_item = "All";
		inline static std::vector<std::string> search_categories;
		inline static std::vector<std::unique_ptr<TextureStructure>> image_vec;
	};
	inline static bool images_loaded = false;

	struct tune
	{
		inline static ImGuiTextFilter filter = "";
		inline static std::string selected_item = "All";
		inline static std::vector<std::string> search_categories;
		inline static std::vector<std::unique_ptr<TextureStructure>> image_vec;
	};

	struct unlimited_nitro
	{
		inline static bool enabled = false;
		inline static bool comp_added = false;
	};

	inline static std::vector<std::string>(handling_flag_names) = // 32 flags
	{
		"1G_BOOST","2G_BOOST","NPC_ANTI_ROLL","NPC_NEUTRAL_HANDL","NO_HANDBRAKE","STEER_REARWHEELS","HB_REARWHEEL_STEER","ALT_STEER_OPT",
		"WHEEL_F_NARROW2","WHEEL_F_NARROW","WHEEL_F_WIDE","WHEEL_F_WIDE2","WHEEL_R_NARROW2","WHEEL_R_NARROW","WHEEL_R_WIDE","WHEEL_R_WIDE2",
		"HYDRAULIC_GEOM","HYDRAULIC_INST","HYDRAULIC_NONE","NOS_INST","OFFROAD_ABILITY","OFFROAD_ABILITY2","HALOGEN_LIGHTS","PROC_REARWHEEL_1ST",
		"USE_MAXSP_LIMIT","LOW_RIDER","STREET_RACER","SWINGING_CHASSIS","Unused 1","Unused 2","Unused 3","Unused 4"
	};

	inline static std::vector<std::string>(model_flag_names) = // 32 flags
	{
		"IS_VAN","IS_BUS","IS_LOW","IS_BIG","REVERSE_BONNET","HANGING_BOOT","TAILGATE_BOOT","NOSWING_BOOT","NO_DOORS","TANDEM_SEATS",
		"SIT_IN_BOAT","CONVERTIBLE","NO_EXHAUST","DOUBLE_EXHAUST","NO1FPS_LOOK_BEHIND","FORCE_DOOR_CHECK","AXLE_F_NOTILT","AXLE_F_SOLID","AXLE_F_MCPHERSON",
		"AXLE_F_REVERSE","AXLE_R_NOTILT","AXLE_R_SOLID","AXLE_R_MCPHERSON","AXLE_R_REVERSE","IS_BIKE","IS_HELI","IS_PLANE","IS_BOAT","BOUNCE_PANELS",
		"DOUBLE_RWHEELS","FORCE_GROUND_CLEARANCE","IS_HATCHBAC1K"
	};

public:
	static void AddComponent(const std::string& component, const bool display_message = true);
	static void RemoveComponent(const std::string& component, const bool display_message = true);
	static std::string GetNameFromModel(int model);
	static int GetModelFromName(const char* name);
	static void SpawnVehicle(std::string& name);
	static int GetRandomTrainIdForModel(int model);
	static void ParseVehiclesIDE();
	static void ParseCarcolsDAT();
	static void GenerateHandlingDataFile(int phandling);
	Vehicle();
	~Vehicle();
	static void Draw();
};

