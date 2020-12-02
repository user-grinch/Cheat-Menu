#include "pch.h"
#include "Teleport.h"
#include "Menu.h"

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

Teleport::Teleport()
{

	json.LoadJsonData(search_categories, selected_item);

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
			{
				player->m_pVehicle->Teleport(STeleport::pos, false);
			}
			else
			{
				player->Teleport(STeleport::pos, false);
			}

			STeleport::_bool = false;
			Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(player), false);
			Command<Commands::SET_CAMERA_BEHIND_PLAYER>();
		}
		
		if (quick_teleport)
		{
			if (Ui::HotKeyPressed(Menu::hotkey::quick_tp)
				&& ((CTimer::m_snTimeInMilliseconds - quick_teleport_timer) > 500))
			{
				quick_teleport_timer = CTimer::m_snTimeInMilliseconds;
				TeleportPlayer(true);
			}
		}
	};

	Events::shutdownRwEvent += []
	{
		config.SetValue("quick_teleport", quick_teleport);
	};
}

void Teleport::TeleportPlayer(bool get_marker, CVector* pos, short interior_id)
{
	CPlayerPed* player = FindPlayerPed();
	
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

		Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(player), true);
	}

	CStreaming::LoadScene(&Teleport::STeleport::pos);
	CStreaming::LoadSceneCollision(&Teleport::STeleport::pos);
	CStreaming::LoadAllRequestedModels(false);

	if (player->m_pVehicle)
	{
		player->m_pVehicle->Teleport(CVector(pos->x, pos->y, pos->z), false);
		Command<Commands::SET_VEHICLE_AREA_VISIBLE>(CPools::GetVehicleRef(player->m_pVehicle), interior_id); // setvehicleinterior
	}
	else
	{
		player->Teleport(CVector(pos->x, pos->y, pos->z), false);
	}

	Command<Commands::SET_CHAR_AREA_VISIBLE>(CPools::GetPedRef(player), interior_id); // setcharinterior
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
				Ui::CheckboxWithHint("Quick teleport", &quick_teleport,
					(std::string("Teleport to marker using ") + Ui::GetHotKeyNameString(Menu::hotkey::quick_tp)).c_str());

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
						pos.z = std::stof(temp);

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
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

Teleport::~Teleport()
{
};

