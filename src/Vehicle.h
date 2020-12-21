#include "NeonAPI.h"
#include "Paint.h"

#pragma once
class Vehicle : NeonAPI, Paint
{
private:
	static bool bike_fly;
	static bool dont_fall_bike;
	static bool veh_heavy;
	static bool veh_watertight;
	static bool veh_nodmg;
	static int door_menu_button;
	static std::string door_names[6];

	static bool lock_speed;
	static float lock_speed_val;

	static std::map<int, std::string> vehicle_ide;
	static std::vector<std::vector<float>> carcols_color_values;
	static std::map<std::string, std::vector<int>> carcols_car_data;

	struct color
	{
		static bool material_filter;
		static int radio_btn;
		static bool show_all;
		static float color_picker[3];
	};

	struct neon
	{
		static float color_picker[3];
		static bool rainbow;
		static uint rainbow_timer;
		static bool traffic;
		static uint traffic_timer;
	};

	struct spawner 
	{
		static ImGuiTextFilter filter;
		static std::string selected_item;
		static std::vector<std::string> search_categories;
		static std::vector<std::unique_ptr<TextureStructure>> image_vec;
		static bool spawn_inside;
		static bool spawn_in_air;
		static char license_text[9];
	};

	struct texture9
	{
		static ImGuiTextFilter filter;
		static std::string selected_item;
		static std::vector<std::string> search_categories;
		static std::vector<std::unique_ptr<TextureStructure>> image_vec;
	};

	struct tune
	{
		static ImGuiTextFilter filter;
		static std::string selected_item;
		static std::vector<std::string> search_categories;
		static std::vector<std::unique_ptr<TextureStructure>> image_vec;
	};

	struct unlimited_nitro
	{
		static bool enabled;
		static bool comp_added;
	};
	
	static void AddComponent(const std::string& component, const bool display_message = true);
	static void RemoveComponent(const std::string& component, const bool display_message = true);
	static std::string GetNameFromModel(int model);
	static void SpawnVehicle(std::string &name);
	static int GetRandomTrainIdForModel(int model);
	static void ParseVehiclesIDE();
	static void ParseCarcolsDAT();
	static void GenerateHandlingDataFile(int phandling);
	
public:
	Vehicle();
	~Vehicle();
	static void Main();
};

