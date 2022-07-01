#include "pch.h"
#include "teleport.h"
#include "menu.h"
#include "widget.h"
#include "util.h"

#ifdef GTASA
// FlA
tRadarTrace* CRadar::ms_RadarTrace = reinterpret_cast<tRadarTrace*>(patch::GetPointer(0x5838B0 + 2));

void Teleport::FetchRadarSpriteData()
{
    static int maxSprites = *(uint*)0x5D5870;
    uint timer = CTimer::m_snTimeInMilliseconds;
    static uint lastUpdated = timer;

    // Update the radar list each 5 seconds
    if (timer - lastUpdated < 5000)
    {
        return;
    }

    m_tpData.m_pData->RemoveTable("Radar");
    for (int i = 0; i != maxSprites; ++i)
    {
        CVector pos = CRadar::ms_RadarTrace[i].m_vecPos;
        std::string sprite = std::to_string(CRadar::ms_RadarTrace[i].m_nRadarSprite);
        std::string keyName = m_SpriteData.Get<std::string>(sprite.c_str(), "Unknown");
        keyName += ", " + Util::GetLocationName(&pos);
        std::string key = "Radar." + keyName;
        m_tpData.m_pData->Set(key.c_str(), std::format("0, {}, {}, {}", pos.x, pos.y, pos.z));
    }
    lastUpdated = timer;
}
#endif

void Teleport::Init()
{
    m_bQuickTeleport = gConfig.Get("Features.QuickTeleport", false);

    Events::processScriptsEvent += []
    {
        if ((QuickTP::m_bEnabled == true) && ((CTimer::m_snTimeInMilliseconds - QuickTP::m_nTimer) > 500))
        {
            CPlayerPed* player = FindPlayerPed();

#ifdef GTASA
            CEntity* player_entity = FindPlayerEntity(-1);
            QuickTP::m_fPos.z = CWorld::FindGroundZFor3DCoord(QuickTP::m_fPos.x, QuickTP::m_fPos.y,
                    QuickTP::m_fPos.z + 100.0f, nullptr, &player_entity) + 1.0f;
#else
            QuickTP::m_fPos.z = CWorld::FindGroundZFor3DCoord(QuickTP::m_fPos.x, QuickTP::m_fPos.y,
                    QuickTP::m_fPos.z + 100.0f, nullptr) + 1.0f;
#endif
            CVehicle* pVeh = player->m_pVehicle;

            if (pVeh && BY_GAME(player->m_nPedFlags.bInVehicle, player->m_pVehicle, player->m_pVehicle))
            {
                BY_GAME(pVeh->Teleport(QuickTP::m_fPos, false), pVeh->Teleport(QuickTP::m_fPos), player->Teleport(QuickTP::m_fPos));
            }
            else
            {
                BY_GAME(player->Teleport(QuickTP::m_fPos, false), player->Teleport(QuickTP::m_fPos), player->Teleport(QuickTP::m_fPos));
            }

            QuickTP::m_bEnabled = false;
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

        if (targetBlip.m_nRadarSprite != RADAR_SPRITE_WAYPOINT)
        {
            SetHelpMessage(TEXT("Teleport.TargetBlipText"));
            return;
        }
        CEntity* pPlayerEntity = FindPlayerEntity(-1);
        pos = targetBlip.m_vecPos;
        pos.z = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr, &pPlayerEntity) + 500.f;

        QuickTP::m_fPos = pos;
        QuickTP::m_nTimer = CTimer::m_snTimeInMilliseconds;
        QuickTP::m_bEnabled = true;
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
        pPlayer->m_nAreaCode = interior_id;
#endif
        pVeh->Teleport(pos);
    }
    else
    {
        pPlayer->Teleport(pos);
    }
#endif

#if defined GTASA || defined GTAVC
    pPlayer->m_nAreaCode = interior_id;
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
        m_tpData.m_pData->RemoveKey("Custom", key.c_str());
        SetHelpMessage(TEXT("Teleport.LocationRemoved"));
        m_tpData.m_pData->Save();
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
                if (Widget::Checkbox(TEXT("Teleport.QuickTeleport"), &m_bQuickTeleport,
                                        std::string(TEXT_S("Teleport.QuickTeleportHint") 
                                                    + quickTeleport.GetNameString()).c_str()))
                {
                    gConfig.Set("Features.QuickTeleport", m_bQuickTeleport);
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

                if (ImGui::Button(TEXT("Teleport.TeleportToCoord"), Widget::CalcSize(2)))
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
                if (ImGui::Button(TEXT("Teleport.TeleportMarker"), Widget::CalcSize(2)))
                {
                    TeleportPlayer(true);
                }
#else
                if (ImGui::Button(TEXT("Teleport.TeleportCenter"), Widget::CalcSize(2)))
                {
                    TeleportPlayer(false, CVector(0, 0, 23));
                }
#endif
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(TEXT("Window.LocationsTab")))
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
                if (ImGui::Button(TEXT("Teleport.AddLocation"), Widget::CalcSize()))
                {
                    std::string key = std::string("Custom.") + m_nLocationBuffer;
                    m_tpData.m_pData->Set(key.c_str(), ("0, " + std::string(m_nInputBuffer)));

    #ifdef GTASA
                    // Clear the Radar coordinates
                    m_tpData.m_pData->RemoveTable("Radar");
    #endif

                    m_tpData.m_pData->Save();
                }
            }

            ImGui::Spacing();
            Widget::DataList(m_tpData, TeleportToLocation, RemoveTeleportEntry);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
