#include "pch.h"
#include "Teleport.h"
#include "Menu.h"
#include "Ui.h"
#include "Util.h"

#ifdef GTASA
// FlA
tRadarTrace* CRadar::ms_RadarTrace = reinterpret_cast<tRadarTrace*>(patch::GetPointer(0x5838B0 + 2));

void Teleport::FetchRadarSpriteData()
{
	uint cur_timer = CTimer::m_snTimeInMilliseconds;
	static uint timer = cur_timer;

	// Update the radar list each 5 seconds
	if (cur_timer - timer < 5000)
	{
		return;
	}

	tp_data.m_pJson->m_Data.erase("Radar");

	// 175 is the max number of sprites, FLA can increase this limit, might need to update this
	for (int i = 0; i != 175; ++i)
	{
		CVector pos = CRadar::ms_RadarTrace[i].m_vPosition;
		uchar sprite = CRadar::ms_RadarTrace[i].m_nBlipSprite;
		auto sprite_name = m_SpriteJson.m_Data[std::to_string(sprite)].get<std::string>();
		std::string key_name = sprite_name + ", " + Util::GetLocationName(&pos);

		tp_data.m_pJson->m_Data["Radar"][key_name] = "0, " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " +
			std::to_string(pos.z);

		/*
			"Radar" : {
				"key_name" : "0, x, y, z",
			}
		*/
	}
}
#endif

Teleport::Teleport()
{
	m_bQuickTeleport = config.GetValue("quick_teleport", false);

	Events::processScriptsEvent += []
	{
		if ((m_Teleport::m_bEnabled == true) && ((CTimer::m_snTimeInMilliseconds - m_Teleport::m_nTimer) > 500))
		{
			CPlayerPed* player = FindPlayerPed();

			#ifdef GTASA
			CEntity* player_entity = FindPlayerEntity(-1);
			m_Teleport::m_fPos.z = CWorld::FindGroundZFor3DCoord(m_Teleport::m_fPos.x, m_Teleport::m_fPos.y,
			                                                 m_Teleport::m_fPos.z + 100.0f, nullptr, &player_entity) + 1.0f;
			#elif GTAVC
			m_Teleport::m_fPos.z = CWorld::FindGroundZFor3DCoord(m_Teleport::m_fPos.x, m_Teleport::m_fPos.y,
			                                                 m_Teleport::m_fPos.z + 100.0f, nullptr) + 1.0f;
			#endif											
			CVehicle* pVeh = player->m_pVehicle;

			if (pVeh && BY_GAME(player->m_nPedFlags.bInVehicle, player->m_pVehicle))
			{
				BY_GAME(pVeh->Teleport(m_Teleport::m_fPos, false), pVeh->Teleport(m_Teleport::m_fPos));
			}
			else
			{
				BY_GAME(player->Teleport(m_Teleport::m_fPos, false), player->Teleport(m_Teleport::m_fPos));
			}

			m_Teleport::m_bEnabled = false;
			Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(player), false);
			Command<Commands::RESTORE_CAMERA_JUMPCUT>();
			TheCamera.Fade(0, 1);
		}

		if (m_bQuickTeleport)
		{
			if (Ui::HotKeyPressed(Menu::m_HotKeys::quickTeleport)
				&& ((CTimer::m_snTimeInMilliseconds - m_nQuickTeleportTimer) > 500))
			{
				m_nQuickTeleportTimer = CTimer::m_snTimeInMilliseconds;
				TeleportPlayer(true);
			}
		}
	};
}

void Teleport::TeleportPlayer(bool get_marker, CVector pos, int interior_id)
{
	CPlayerPed* pPlayer = FindPlayerPed();
	CVehicle* pVeh = pPlayer->m_pVehicle;
	
	#ifdef GTASA
	if (get_marker)
	{
		tRadarTrace targetBlip = CRadar::ms_RadarTrace[LOWORD(FrontEndMenuManager.m_nTargetBlipIndex)];

		if (targetBlip.m_nBlipSprite != RADAR_SPRITE_WAYPOINT)
		{
			SetHelpMessage("Target blip not found. You need to place it on the map first.", false, false, false);
			return;
		}
		CEntity* pPlayerEntity = FindPlayerEntity(-1);
		pos = targetBlip.m_vPosition;
		pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr, &pPlayerEntity) + 50.f;

		m_Teleport::m_fPos = pos;
		m_Teleport::m_nTimer = CTimer::m_snTimeInMilliseconds;
		m_Teleport::m_bEnabled = true;
		TheCamera.Fade(0, 0);
		Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(pPlayer), true);
	}
	#endif

	CStreaming::LoadScene(&pos);
	CStreaming::LoadSceneCollision(&pos);
	CStreaming::LoadAllRequestedModels(false);

	#ifdef GTASA
	if (pVeh && pPlayer->m_nPedFlags.bInVehicle)
	{
		pVeh->Teleport(pos, false);
		
		if (pVeh->m_nVehicleClass == VEHICLE_BIKE)
			reinterpret_cast<CBike*>(pVeh)->PlaceOnRoadProperly();
		else if (pVeh->m_nVehicleClass != VEHICLE_BOAT)
			reinterpret_cast<CAutomobile*>(pVeh)->PlaceOnRoadProperly();

		BY_GAME(pVeh->m_nAreaCode, pVeh->m_nInterior) = interior_id;
	}
	else
	{
		pPlayer->Teleport(pos, false);
	}
	#elif GTAVC
	if (pVeh && pPlayer->m_pVehicle)
	{
		BY_GAME(pVeh->m_nAreaCode, pVeh->m_nInterior) = interior_id;
		pVeh->Teleport(pos);
	}
	else
	{
		pPlayer->Teleport(pos);
	}
	#endif

	BY_GAME(pPlayer->m_nAreaCode, pPlayer->m_nInterior) = interior_id;
	Command<Commands::SET_AREA_VISIBLE>(interior_id);
}

void Teleport::TeleportToLocation(std::string& rootkey, std::string& bLocName, std::string& loc)
{
	try
	{
		int dimension = 0;
		CVector pos;
		sscanf(loc.c_str(), "%d,%f,%f,%f", &dimension, &pos.x, &pos.y, &pos.z);
		TeleportPlayer(false, pos, dimension);
	}
	catch (...)
	{
		SetHelpMessage("Invalid location", false, false, false);
	}
}

void Teleport::RemoveTeleportEntry(std::string& category, std::string& key, std::string& val)
{
	if (category == "Custom")
	{
		tp_data.m_pJson->m_Data["Custom"].erase(key);
		SetHelpMessage("Location removed", false, false, false);
		tp_data.m_pJson->WriteToDisk();
	}
	else 
	{
		SetHelpMessage("You can only remove custom location", false, false, false);
	}
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
				#ifdef GTASA
				if (Ui::CheckboxWithHint("Quick teleport", &m_bQuickTeleport,
				                         (std::string("Teleport to the location of your radar\ntarget blip using ") 
											 + Ui::GetHotKeyNameString(Menu::m_HotKeys::quickTeleport)).c_str()))
				{
					config.SetValue("quick_teleport", m_bQuickTeleport);
				}
				#endif	
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

				if (ImGui::Button("Teleport to coord", Ui::GetSize(2)))
				{
					CVector pos;

					try
					{
						sscanf(m_nInputBuffer,"%f,%f,%f", &pos.x, &pos.y, &pos.z);
						pos.z += 1.0f;
						TeleportPlayer(false, pos);
					}
					catch (...)
					{
						SetHelpMessage("Invalid coordinate", false, false, false);
					}
				}
				ImGui::SameLine();
				#ifdef GTASA
				if (ImGui::Button("Teleport to marker", Ui::GetSize(2)))
				{
					TeleportPlayer(true);
				}
				#else
				if (ImGui::Button("Teleport to map center", Ui::GetSize(2)))
				{
					TeleportPlayer(false, CVector(0, 0, 23));
				}
				#endif
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Search"))
		{
			#ifdef GTASA
			FetchRadarSpriteData();
			#endif

			ImGui::Spacing();
			Ui::DrawJSON(tp_data, TeleportToLocation,RemoveTeleportEntry);
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
				tp_data.m_pJson->m_Data["Custom"][m_nLocationBuffer] = ("0, " + std::string(m_nInputBuffer));

				#ifdef GTASA
				// Clear the Radar coordinates
				tp_data.m_pJson->m_Data.erase("Radar");
				tp_data.m_pJson->m_Data["Radar"] = {};
				#endif

				tp_data.m_pJson->WriteToDisk();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
