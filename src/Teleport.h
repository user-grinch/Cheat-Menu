#pragma once
#include <vector>
#include "pch.h"

class Teleport
{
private:
	static bool insert_coord;
	static bool quick_teleport;
	static char input_buffer[INPUT_BUFFER_SIZE];
	static CJson json;
	static ImGuiTextFilter filter;
	static std::vector<std::string> search_categories;
	static std::string selected_item;
	static char location_buffer[INPUT_BUFFER_SIZE];

	struct STeleport
	{
		static bool _bool;
		static CVector pos;
		static uint timer;
	};

	static CJson sprite_name_json;

	/*
		Generates radar sprite coordinates on the fly.
		Shouldn't get saved in 'teleport.json', needs to be cleared at game shutdown.
	*/
	static void FetchRadarSpriteData();

protected:
	Teleport();
public:

	static void Draw();
	static void TeleportPlayer(bool get_marker = false, CVector pos = CVector(0, 0, 0), short interior_id = 0);
	static void TeleportToLocation(std::string& rootkey, std::string& loc_name, std::string& loc);
	static void RemoveTeleportEntry(std::string& rootkey, std::string& key, std::string& val);

};