#pragma once
#include <vector>
#include "pch.h"

class Teleport
{
private:
	inline static bool insert_coord = false;
	inline static bool quick_teleport = false;
	inline static char input_buffer[INPUT_BUFFER_SIZE] = "";
	inline static SearchData tp_data{ "teleport" };
	inline static char location_buffer[INPUT_BUFFER_SIZE] = "";
	inline static uint quick_teleport_timer = 0;
	
	struct STeleport
	{
		inline static bool _bool = false;
		inline static CVector pos = { -1, -1, -1 };
		inline static uint timer = 0;
	};

	inline static CJson sprite_name_json = CJson("radar sprite");

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
	static void TeleportToLocation(std::string& rootkey, std::string& bLocName, std::string& loc);
	static void RemoveTeleportEntry(std::string& rootkey, std::string& key, std::string& val);

};