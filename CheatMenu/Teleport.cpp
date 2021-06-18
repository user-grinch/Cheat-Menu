#include "pch.h"
#include "Teleport.h"
#include "Menu.h"
#include "Ui.h"
#include "Util.h"

// FlA
tRadarTrace* CRadar::ms_RadarTrace = reinterpret_cast<tRadarTrace*>(patch::GetPointer(0x5838B0 + 2));

void Teleport::FetchRadarSpriteData()
{
	uint cur_timer = CTimer::m_snTimeInMilliseconds;
	static uint timer = cur_timer;

	// Update the radar list each 5 seconds
	if (cur_timer - timer < 5000)
		return;

	tp_data.m_Json.m_Data.erase("Radar");

	// 175 is the max number of sprites, FLA can increase this limit, might need to update this
	for (int i = 0; i != 175; ++i)
	{
		CVector pos = CRadar::ms_RadarTrace[i].m_vPosition;
		uchar sprite = CRadar::ms_RadarTrace[i].m_nBlipSprite;
		auto sprite_name = m_SpriteJson.m_Data[std::to_string(sprite)].get<std::string>();
		std::string key_name = sprite_name + ", " + Util::GetLocationName(&pos);

		tp_data.m_Json.m_Data["Radar"][key_name] = "0, " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " +
			std::to_string(pos.z);

		/*
			"Radar" : {
				"key_name" : "0, x, y, z",
			}
		*/
	}
}

Teleport::Teleport()
{
	tp_data.m_Json.LoadData(tp_data.m_Categories, tp_data.m_Selected);
	m_bQuickTeleport = config.GetValue("quick_teleport", false);

	Events::processScriptsEvent += []
	{
		if ((m_Teleport.m_bEnabled == true) && ((CTimer::m_snTimeInMilliseconds - m_Teleport.m_nTimer) > 500))
		{
			CPlayerPed* player = FindPlayerPed();

			CEntity* player_entity = FindPlayerEntity(-1);
			m_Teleport.m_fPos.z = CWorld::FindGroundZFor3DCoord(m_Teleport.m_fPos.x, m_Teleport.m_fPos.y,
			                                                 m_Teleport.m_fPos.z + 100.0f, nullptr, &player_entity) + 1.0f;
			CVehicle* pVeh = player->m_pVehicle;

			if (pVeh && player->m_nPedFlags.bInVehicle)
				pVeh->Teleport(m_Teleport.m_fPos, false);
			else
				player->Teleport(m_Teleport.m_fPos, false);

			m_Teleport.m_bEnabled = false;
			Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(player), false);
			Command<Commands::RESTORE_CAMERA_JUMPCUT>();
			TheCamera.Fade(0, 1);
		}

		if (m_bQuickTeleport)
		{
			if (Ui::HotKeyPressed(Menu::m_HotKeys.quickTeleport)
				&& ((CTimer::m_snTimeInMilliseconds - m_nQuickTeleportTimer) > 500))
			{
				m_nQuickTeleportTimer = CTimer::m_snTimeInMilliseconds;
				TeleportPlayer(true);
			}
		}
	};
}

void Teleport::TeleportPlayer(bool get_marker, CVector pos, short interior_id)
{
	CPlayerPed* pPlayer = FindPlayerPed();
	CVehicle* pVeh = pPlayer->m_pVehicle;

	if (get_marker)
	{
		tRadarTrace targetBlip = CRadar::ms_RadarTrace[LOWORD(FrontEndMenuManager.m_nTargetBlipIndex)];

		if (targetBlip.m_nBlipSprite != RADAR_SPRITE_WAYPOINT)
		{
			CHud::SetHelpMessage("No blip found", false, false, false);
			return;
		}
		CEntity* pPlayerEntity = FindPlayerEntity(-1);
		pos = targetBlip.m_vPosition;
		pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr, &pPlayerEntity) + 50.f;

		m_Teleport.m_fPos = pos;
		m_Teleport.m_nTimer = CTimer::m_snTimeInMilliseconds;
		m_Teleport.m_bEnabled = true;
		TheCamera.Fade(0, 0);
		Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(pPlayer), true);
	}

	CStreaming::LoadScene(&pos);
	CStreaming::LoadSceneCollision(&pos);
	CStreaming::LoadAllRequestedModels(false);

	if (pVeh && pPlayer->m_nPedFlags.bInVehicle)
	{
		pVeh->Teleport(pos, false);

		if (pVeh->m_nVehicleClass == VEHICLE_BIKE)
			reinterpret_cast<CBike*>(pVeh)->PlaceOnRoadProperly();
		else if (pVeh->m_nVehicleClass != VEHICLE_BOAT)
			reinterpret_cast<CAutomobile*>(pVeh)->PlaceOnRoadProperly();

		pVeh->m_nAreaCode = interior_id;
	}
	else
		pPlayer->Teleport(pos, false);

	pPlayer->m_nAreaCode = interior_id;
	Command<Commands::SET_AREA_VISIBLE>(interior_id);
}

void Teleport::TeleportToLocation(std::string& rootkey, std::string& bLocName, std::string& loc)
{
	try
	{
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

		TeleportPlayer(false, pos, static_cast<short>(interior));
	}
	catch (...)
	{
		CHud::SetHelpMessage("Invalid location", false, false, false);
	}
}

void Teleport::RemoveTeleportEntry(std::string& category, std::string& key, std::string& val)
{
	if (category == "Custom")
	{
		tp_data.m_Json.m_Data["Custom"].erase(key);
		CHud::SetHelpMessage("Location removed", false, false, false);
		tp_data.m_Json.WriteToDisk();
	}
	else CHud::SetHelpMessage("You can only remove custom location", false, false, false);
}

void Teleport::Draw()
{
	if (ImGui::BeginTabBar("Teleport", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		ImGui::Spacing();
		if (ImGui::BeginTabItem("Teleport"))
		{
			ImGui::Spacing();
			if (ImGui::BeginChild("Teleport Child"))
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::Checkbox("Insert coordinates", &m_bInsertCoord);
				ImGui::NextColumn();
				if (Ui::CheckboxWithHint("Quick teleport", &m_bQuickTeleport,
				                         (std::string("Teleport to marker using ") + Ui::GetHotKeyNameString(
					                         Menu::m_HotKeys.quickTeleport)).c_str()))
				{
					config.SetValue("quick_teleport", m_bQuickTeleport);
				}

				ImGui::Columns(1);
				ImGui::Spacing();

				if (m_bInsertCoord)
				{
					CVector pos = FindPlayerPed()->GetPosition();

					strcpy(m_nInputBuffer,
					       (std::to_string(static_cast<int>(pos.x)) + ", " + std::to_string(static_cast<int>(pos.y)) +
						       ", " + std::to_string(static_cast<int>(pos.z))).c_str());
				}

				ImGui::InputTextWithHint("Coordinates", "x, y, z", m_nInputBuffer, IM_ARRAYSIZE(m_nInputBuffer));

				ImGui::Spacing();

				if (ImGui::Button("Teleport to bCoord", Ui::GetSize(2)))
				{
					std::stringstream ss(m_nInputBuffer);
					std::string temp;
					CVector pos(0, 0, 0);

					try
					{
						getline(ss, temp, ',');
						pos.x = std::stof(temp);

						getline(ss, temp, ',');
						pos.y = std::stof(temp);

						getline(ss, temp, ',');
						pos.z = std::stof(temp) + 1.0f;

						TeleportPlayer(false, pos);
					}
					catch (...)
					{
						CHud::SetHelpMessage("Invalid coordinate", false, false, false);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Teleport to marker", Ui::GetSize(2)))
					TeleportPlayer(true);

				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Search"))
		{
			FetchRadarSpriteData();
			ImGui::Spacing();
			Ui::DrawJSON(tp_data.m_Json, tp_data.m_Categories, tp_data.m_Selected, tp_data.m_Filter, &TeleportToLocation,
			             &RemoveTeleportEntry);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Custom"))
		{
			ImGui::Spacing();
			ImGui::InputTextWithHint("Location", "Groove Street", m_nLocationBuffer, IM_ARRAYSIZE(m_nInputBuffer));
			ImGui::InputTextWithHint("Coordinates", "x, y, z", m_nInputBuffer, IM_ARRAYSIZE(m_nInputBuffer));
			ImGui::Spacing();
			if (ImGui::Button("Add location", Ui::GetSize()))
			{
				tp_data.m_Json.m_Data["Custom"][m_nLocationBuffer] = ("0, " + std::string(m_nInputBuffer));

				// Clear the Radar coordinates
				tp_data.m_Json.m_Data.erase("Radar");
				tp_data.m_Json.m_Data["Radar"] = {};

				tp_data.m_Json.WriteToDisk();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
