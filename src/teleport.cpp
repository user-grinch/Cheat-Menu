#include "pch.h"
#include "teleport.h"
#include "menu.h"
#include "ui.h"
#include "util.h"

#ifdef GTASA
// FlA
tRadarTrace* CRadar::ms_RadarTrace = reinterpret_cast<tRadarTrace*>(patch::GetPointer(0x5838B0 + 2));

void Teleport::FetchRadarSpriteData()
{
    uint cur_timer = CTimer::m_snTimeInMilliseconds;
    static uint timer = cur_timer;
    static int maxSprites = *(uint*)0x5D5870;

    // Update the radar list each 5 seconds
    if (cur_timer - timer < 5000)
    {
        return;
    }

    m_tpData.m_pJson->m_Data.erase("Radar");
    for (int i = 0; i != maxSprites; ++i)
    {
        CVector pos = CRadar::ms_RadarTrace[i].m_vPosition;
        uchar sprite = CRadar::ms_RadarTrace[i].m_nBlipSprite;
        auto sprite_name = m_SpriteJson.m_Data[std::to_string(sprite)].get<std::string>();
        std::string key_name = sprite_name + ", " + Util::GetLocationName(&pos);

        m_tpData.m_pJson->m_Data["Radar"][key_name] = "0, " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " +
                std::to_string(pos.z);

        /*
        	"Radar" : {
        		"key_name" : "0, x, y, z",
        	}
        */
    }
}
#endif

void Teleport::Init()
{
    m_bQuickTeleport = gConfig.GetValue("quick_teleport", false);

    Events::processScriptsEvent += []
    {
        if ((m_Teleport::m_bEnabled == true) && ((CTimer::m_snTimeInMilliseconds - m_Teleport::m_nTimer) > 500))
        {
            CPlayerPed* player = FindPlayerPed();

#ifdef GTASA
            CEntity* player_entity = FindPlayerEntity(-1);
            m_Teleport::m_fPos.z = CWorld::FindGroundZFor3DCoord(m_Teleport::m_fPos.x, m_Teleport::m_fPos.y,
                    m_Teleport::m_fPos.z + 100.0f, nullptr, &player_entity) + 1.0f;
#else
            m_Teleport::m_fPos.z = CWorld::FindGroundZFor3DCoord(m_Teleport::m_fPos.x, m_Teleport::m_fPos.y,
                    m_Teleport::m_fPos.z + 100.0f, nullptr) + 1.0f;
#endif
            CVehicle* pVeh = player->m_pVehicle;

            if (pVeh && BY_GAME(player->m_nPedFlags.bInVehicle, player->m_pVehicle, player->m_pVehicle))
            {
                BY_GAME(pVeh->Teleport(m_Teleport::m_fPos, false), pVeh->Teleport(m_Teleport::m_fPos), player->Teleport(m_Teleport::m_fPos));
            }
            else
            {
                BY_GAME(player->Teleport(m_Teleport::m_fPos, false), player->Teleport(m_Teleport::m_fPos), player->Teleport(m_Teleport::m_fPos));
            }

            m_Teleport::m_bEnabled = false;
            Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(player), false);
            Command<Commands::RESTORE_CAMERA_JUMPCUT>();
            TheCamera.Fade(0, 1);
        }

        if (m_bQuickTeleport)
        {
            if (quickTeleport.Pressed()
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
            SetHelpMessage(TEXT("Teleport.TargetBlipText"));
            return;
        }
        CEntity* pPlayerEntity = FindPlayerEntity(-1);
        pos = targetBlip.m_vPosition;
        pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr, &pPlayerEntity) + 500.f;

        m_Teleport::m_fPos = pos;
        m_Teleport::m_nTimer = CTimer::m_snTimeInMilliseconds;
        m_Teleport::m_bEnabled = true;
        TheCamera.Fade(0, 0);
        Command<Commands::FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION>(CPools::GetPedRef(pPlayer), true);
    }
#endif

#ifdef GTA3
    CStreaming::LoadScene(pos);
#else
    CStreaming::LoadScene(&pos);
    CStreaming::LoadSceneCollision(&pos);
#endif
    CStreaming::LoadAllRequestedModels(false);

#ifdef GTASA

    if (pVeh && pPlayer->m_nPedFlags.bInVehicle)
    {

        if (CModelInfo::IsTrainModel(pVeh->m_nModelIndex))
        {
            CVector vehPos = pVeh->GetPosition();
            Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(pPlayer), vehPos.x, vehPos.y, vehPos.z + 2.0f);

            if (DistanceBetweenPoints(pos, vehPos) > 100.0f)
            {
                Command<Commands::DELETE_ALL_TRAINS>();
            }

            pPlayer->Teleport(pos, false);
        }  
        else
        {
            pVeh->Teleport(pos, false);

            if (pVeh->m_nVehicleClass == VEHICLE_BIKE)
            {
                reinterpret_cast<CBike*>(pVeh)->PlaceOnRoadProperly();
            }
            else if (pVeh->m_nVehicleClass != VEHICLE_BOAT)
            {
                reinterpret_cast<CAutomobile*>(pVeh)->PlaceOnRoadProperly();
            }

            pVeh->m_nAreaCode = interior_id;
        }
    }
    else
    {
        pPlayer->Teleport(pos, false);
    }
#else
    if (pVeh && pPlayer->m_pVehicle)
    {
#ifdef GTAVC
        pPlayer->m_nInterior = interior_id;
#endif
        pVeh->Teleport(pos);
    }
    else
    {
        pPlayer->Teleport(pos);
    }
#endif

#if defined GTASA || defined GTAVC
    BY_GAME(pPlayer->m_nAreaCode, pPlayer->m_nInterior, NULL) = interior_id;
    Command<Commands::SET_AREA_VISIBLE>(interior_id);
#endif
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
        SetHelpMessage(TEXT("Teleport.InvalidLocation"));
    }
}

void Teleport::RemoveTeleportEntry(std::string& category, std::string& key, std::string& val)
{
    if (category == "Custom")
    {
        m_tpData.m_pJson->m_Data["Custom"].erase(key);
        SetHelpMessage(TEXT("Teleport.LocationRemoved"));
        m_tpData.m_pJson->WriteToDisk();
    }
    else
    {
        SetHelpMessage(TEXT("Teleport.CustomLocationRemoveOnly"));
    }
}

void Teleport::ShowPage()
{
    if (ImGui::BeginTabBar("Teleport", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        ImGui::Spacing();
        if (ImGui::BeginTabItem(TEXT("Window.TeleportPage")))
        {
            ImGui::Spacing();
            if (ImGui::BeginChild("Teleport Child"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Checkbox(TEXT("Teleport.InsertCoord"), &m_bInsertCoord);
                ImGui::NextColumn();
#ifdef GTASA
                if (Ui::CheckboxWithHint(TEXT("Teleport.QuickTeleport"), &m_bQuickTeleport,
                                        std::string(TEXT_S("Teleport.QuickTeleportHint") 
                                                    + quickTeleport.GetNameString()).c_str()))
                {
                    gConfig.SetValue("quick_teleport", m_bQuickTeleport);
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

                ImGui::InputTextWithHint(TEXT("Teleport.Coordinates"), "x, y, z", m_nInputBuffer, IM_ARRAYSIZE(m_nInputBuffer));

                ImGui::Spacing();

                if (ImGui::Button(TEXT("Teleport.TeleportToCoord"), Ui::GetSize(2)))
                {
                    CVector pos{0, 0, 10};

                    try
                    {
                        sscanf(m_nInputBuffer,"%f,%f,%f", &pos.x, &pos.y, &pos.z);
                        pos.z += 1.0f;
                        TeleportPlayer(false, pos);
                    }
                    catch (...)
                    {
                        SetHelpMessage(TEXT("Teleport.InvalidCoord"));
                    }
                }
                ImGui::SameLine();
#ifdef GTASA
                if (ImGui::Button(TEXT("Teleport.TeleportMarker"), Ui::GetSize(2)))
                {
                    TeleportPlayer(true);
                }
#else
                if (ImGui::Button(TEXT("Teleport.TeleportCenter"), Ui::GetSize(2)))
                {
                    TeleportPlayer(false, CVector(0, 0, 23));
                }
#endif
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(TEXT("Window.Search")))
        {
#ifdef GTASA
            FetchRadarSpriteData();
#endif  
            ImGui::Spacing();
            if (ImGui::CollapsingHeader(TEXT("Window.AddNew")))
            {
                ImGui::Spacing();
                ImGui::InputTextWithHint(TEXT("Teleport.Location"), TEXT("Teleport.LocationHint"), m_nLocationBuffer, IM_ARRAYSIZE(m_nInputBuffer));
                ImGui::InputTextWithHint(TEXT("Teleport.Coordinates"), "x, y, z", m_nInputBuffer, IM_ARRAYSIZE(m_nInputBuffer));
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Teleport.AddLocation"), Ui::GetSize()))
                {
                    m_tpData.m_pJson->m_Data["Custom"][m_nLocationBuffer] = ("0, " + std::string(m_nInputBuffer));

    #ifdef GTASA
                    // Clear the Radar coordinates
                    m_tpData.m_pJson->m_Data.erase("Radar");
                    m_tpData.m_pJson->m_Data["Radar"] = {};
    #endif

                    m_tpData.m_pJson->WriteToDisk();
                }
            }

            ImGui::Spacing();
            Ui::DrawJSON(m_tpData, TeleportToLocation,RemoveTeleportEntry);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
