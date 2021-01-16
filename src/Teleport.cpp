#include "pch.h"
#include "Teleport.h"
#include "Menu.h"
#include "Ui.h"
#include "Util.h"

// FlA
tRadarTrace* CRadar::ms_RadarTrace = reinterpret_cast<tRadarTrace *>(patch::GetPointer(0x5838B0 + 2));

bool Teleport::insert_coord = false;
bool Teleport::quick_teleport = false;
char Teleport::input_buffer[INPUT_BUFFER_SIZE] = "";
char Teleport::location_buffer[INPUT_BUFFER_SIZE] = "";

CVector Teleport::STeleport::pos{ -1, -1, -1 };
bool Teleport::STeleport::_bool = false;
uint Teleport::STeleport::timer = 0;
ImGuiTextFilter Teleport::filter = "";
std::vector<std::string> Teleport::search_categories;
std::string Teleport::selected_item = "All";

uint quick_teleport_timer = 0;

CJson Teleport::json = CJson("teleport");
CJson Teleport::sprite_name_json = CJson("radar sprite");

void Teleport::FetchRadarSpriteData()
{
	uint cur_timer = CTimer::m_snTimeInMilliseconds;
	static uint timer = cur_timer;

	// Update the radar list each 5 seconds
	if (cur_timer - timer < 5000)
		return;

	json.data.erase("Radar");

	// 175 is the max number of sprites, FLA can increase this limit, might need to update this
	for (int i = 0; i != 175; ++i)	
	{
		CVector pos = CRadar::ms_RadarTrace[i].m_vPosition;
		uchar sprite = CRadar::ms_RadarTrace[i].m_nBlipSprite;
		std::string sprite_name = sprite_name_json.data[std::to_string(sprite)].get<std::string>();
		std::string key_name = sprite_name + ", " + Util::GetLocationName(&pos);

		json.data["Radar"][key_name] = "0, " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z);

		/*
			"Radar" : {
				"key_name" : "0, x, y, z",
			}
		*/
	}
}

Teleport::Teleport()
{
	json.LoadData(search_categories, selected_item);

	Events::initGameEvent += []
	{
		// Load config data
		quick_teleport = config.GetValue("quick_teleport", false);
	};

	Events::processScriptsEvent += []
	{
		if ((STeleport::_bool == true) && ((CTimer::m_snTimeInMilliseconds - STeleport::timer) > 500))
		{
			CPlayerPed *player = FindPlayerPed();

			CEntity* player_entity = FindPlayerEntity(-1);
			STeleport::pos.z = CWorld::FindGroundZFor3DCoord(Teleport::STeleport::pos.x, Teleport::STeleport::pos.y, STeleport::pos.z + 100.0f, 0, &player_entity) + 1.0f;

			if (player->m_pVehicle)
				player->m_pVehicle->Teleport(STeleport::pos, false);
			else
				player->Teleport(STeleport::pos, false);

			STeleport::_bool = false;
			Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(player), false);
			Command<Commands::SET_CAMERA_BEHIND_PLAYER>();
			TheCamera.Fade(0,1);
		}
		
		if (quick_teleport)
		{
			if (Ui::HotKeyPressed(Menu::hotkeys::quick_tp)
				&& ((CTimer::m_snTimeInMilliseconds - quick_teleport_timer) > 500))
			{
				quick_teleport_timer = CTimer::m_snTimeInMilliseconds;
				TeleportPlayer(true);
			}
		}
	};
}

void Teleport::TeleportPlayer(bool get_marker, CVector* pos, short interior_id)
{
	CPlayerPed *player = FindPlayerPed();
	CVehicle *pVeh = player->m_pVehicle;

	if (get_marker)
	{
		auto target_blip = CRadar::ms_RadarTrace[LOWORD(FrontEndMenuManager.m_nTargetBlipIndex)];


		if (target_blip.m_nBlipSprite != RADAR_SPRITE_WAYPOINT)
		{
			CHud::SetHelpMessage("No blip found", false, false, false);
			return;
		}
		CEntity* player_entity = FindPlayerEntity(-1);
		pos = &(target_blip.m_vPosition);
		pos->z = CWorld::FindGroundZFor3DCoord(pos->x, pos->y, 1000, 0, &player_entity) + 50.f;

		Teleport::STeleport::pos = *pos;
		Teleport::STeleport::timer = CTimer::m_snTimeInMilliseconds;
		Teleport::STeleport::_bool = true;
		TheCamera.Fade(0,0);
		Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(player), true);
	}

	CStreaming::LoadScene(&Teleport::STeleport::pos);
	CStreaming::LoadSceneCollision(&Teleport::STeleport::pos);
	CStreaming::LoadAllRequestedModels(false);

	if (pVeh)
	{
		pVeh->Teleport(CVector(pos->x, pos->y, pos->z), false);

		if (pVeh->m_nVehicleClass == VEHICLE_BIKE)
			reinterpret_cast<CBike*>(pVeh)->PlaceOnRoadProperly();
		else if (pVeh->m_nVehicleClass != VEHICLE_BOAT)
			reinterpret_cast<CAutomobile*>(pVeh)->PlaceOnRoadProperly();
			
		pVeh->m_nAreaCode = interior_id;
	}
	else
		player->Teleport(CVector(pos->x, pos->y, pos->z), false);

	player->m_nAreaCode = interior_id;
	Command<Commands::SET_AREA_VISIBLE>(interior_id);
}

void Teleport::TeleportToLocation(std::string& rootkey, std::string& loc_name,std::string& loc)
{
	try {
		int interior = 0;
		CVector pos;
		std::stringstream ss(loc);
		std::string temp;

		std::getline(ss, temp, ',');
		interior = std::stoi(temp);

		std::getline(ss, temp, ',');
		pos.x = std::stof(temp);

		std::getline(ss, temp, ',');
		pos.y = std::stof(temp);

		std::getline(ss, temp, ',');
		pos.z = std::stof(temp);

		Teleport::TeleportPlayer(false, &pos, static_cast<short>(interior));
	}
	catch (...) {
		CHud::SetHelpMessage("Invalid location", false, false, false);
	}
}

void Teleport::RemoveTeleportEntry(std::string& category, std::string& key, std::string& val)
{
	if (category == "Custom")
	{
		json.data["Custom"].erase(key);
		CHud::SetHelpMessage("Location removed", false, false, false);
		json.WriteToDisk();
	}
	else CHud::SetHelpMessage("You can only remove custom location", false, false, false);
}

void Teleport::Main()
{
	if (ImGui::BeginTabBar("Teleport",ImGuiTabBarFlags_NoTooltip+ImGuiTabBarFlags_FittingPolicyScroll))
	{
		ImGui::Spacing();
		if (ImGui::BeginTabItem("Teleport"))
		{
			ImGui::Spacing();
			if (ImGui::BeginChild("Teleport Child"))
			{
				ImGui::Columns(2, 0, false);
				ImGui::Checkbox("Insert coordinates", &insert_coord);
				ImGui::NextColumn();
				if (Ui::CheckboxWithHint("Quick teleport", &quick_teleport,
					(std::string("Teleport to marker using ") + Ui::GetHotKeyNameString(Menu::hotkeys::quick_tp)).c_str()))
				{
					config.SetValue("quick_teleport", quick_teleport);
				}

				ImGui::Columns(1);
				ImGui::Spacing();

				if (insert_coord)
				{
					CVector pos = FindPlayerPed()->GetPosition();

					strcpy(input_buffer,(std::to_string(int(pos.x)) + ", " + std::to_string(int(pos.y)) + ", " + std::to_string(int(pos.z))).c_str());
				}

				ImGui::InputTextWithHint("Coordinates", "x, y, z", input_buffer, IM_ARRAYSIZE(input_buffer));
				
				ImGui::Spacing();
				
				if (ImGui::Button("Teleport to coord", Ui::GetSize(2)))
				{
					std::stringstream ss(input_buffer);
					std::string temp;
					CVector pos(0,0,0);

					try {
						getline(ss, temp, ',');
						pos.x = std::stof(temp);

						getline(ss, temp, ',');
						pos.y = std::stof(temp);

						getline(ss, temp, ',');
						pos.z = std::stof(temp) + 1.0f;

						Teleport::TeleportPlayer(false,&pos);
					}
					catch (...) {
						CHud::SetHelpMessage("Invalid coordinate", false, false, false);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Teleport to marker", Ui::GetSize(2)))
					Teleport::TeleportPlayer(true);

				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Search"))
		{
			FetchRadarSpriteData();
			ImGui::Spacing();
			Ui::DrawJSON(json, search_categories, selected_item, filter, &TeleportToLocation, &RemoveTeleportEntry);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Custom"))
		{
			ImGui::Spacing();
			ImGui::InputTextWithHint("Location", "Groove Street", location_buffer, IM_ARRAYSIZE(input_buffer));
			ImGui::InputTextWithHint("Coordinates", "x, y, z", input_buffer, IM_ARRAYSIZE(input_buffer));
			ImGui::Spacing();
			if (ImGui::Button("Add location", Ui::GetSize()))
			{
				json.data["Custom"][location_buffer] = ("0, " + std::string(input_buffer));

				// Clear the Radar coordinates
				json.data.erase("Radar");
				json.data["Radar"] = {};

				json.WriteToDisk();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

Teleport::~Teleport()
{
};

