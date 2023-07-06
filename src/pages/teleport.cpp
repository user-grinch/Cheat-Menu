#include "pch.h"
#include "teleport.h"
#include "menu.h"
#include "utils/widget.h"
#include "utils/util.h"

#ifdef GTASA
// FLA
tRadarTrace* ms_RadarTrace = reinterpret_cast<tRadarTrace*>(patch::GetPointer(0x5838B0 + 2));
static int maxSprites = *(uint*)0x5D5870;

void TeleportPage::FetchRadarSpriteData()
{

    uint timer = CTimer::m_snTimeInMilliseconds;
    static uint lastUpdated = timer;

    // Update the radar list each 5 seconds
    if (timer - lastUpdated < 5000)
    {
        return;
    }

    m_locData.m_pData->RemoveTable("Radar");
    for (int i = 0; i != maxSprites; ++i)
    {
        CVector pos = ms_RadarTrace[i].m_vecPos;
        std::string sprite = std::to_string(ms_RadarTrace[i].m_nRadarSprite);
        std::string keyName = m_SpriteData.Get<std::string>(sprite.c_str(), "Unknown");
        keyName += ", " + Util::GetLocationName(&pos);
        std::string key = "Radar." + keyName;
        m_locData.m_pData->Set(key.c_str(), std::format("0, {}, {}, {}", pos.x, pos.y, pos.z));
    }
    lastUpdated = timer;
}
#endif

bool TeleportPage::IsQuickTeleportActive()
{
    return m_bQuickTeleport;
}

TeleportPage& teleportPage = TeleportPage::Get();

TeleportPage::TeleportPage()
    : IPage<TeleportPage>(ePageID::Teleport, "Teleport", true)
{
    Events::initGameEvent += [this]()
    {
        m_bTeleportMarker = gConfig.Get("Features.TeleportMarker", false);
        m_bQuickTeleport = gConfig.Get("Features.QuickTeleport", false);
        m_bSpawnUnderwater = gConfig.Get("Features.SpawnUnderwater", false);
        m_fMapSize.x = gConfig.Get("Game.MapSizeX", 6000.0f);
        m_fMapSize.y = gConfig.Get("Game.MapSizeY", 6000.0f);
    };

    Events::drawingEvent += [this]
    {
        if (m_bTeleportMarker && teleportMarker.Pressed())
        {
            WarpPlayer<eTeleportType::Marker>();
        }

#ifdef GTASA
        if (m_bQuickTeleport && quickTeleport.PressedRealtime())
        {
            static CSprite2d map;
            if (!map.m_pTexture)
            {
                map.m_pTexture = gTextureList.FindRwTextureByName("map");
            }
            ImVec2 screenSz = ImVec2(screen::GetScreenWidth(), screen::GetScreenHeight());
            float size = screenSz.x * screenSz.y / screenSz.x; // keep aspect ratio
            float left = (screenSz.x-size) / 2;
            float right = left+size;
            map.Draw(CRect(left, 0.0f, right, screenSz.y), CRGBA(255, 255, 255, 200));

            // draw sprites on map
            static float sz = SCREEN_MULTIPLIER(12.5f);
            for (int i = 0; i != maxSprites; ++i)
            {
                tRadarTrace &trace = ms_RadarTrace[i];

                if (trace.m_nRadarSprite != RADAR_SPRITE_NONE)
                {
                    CSprite2d &sprite = CRadar::RadarBlipSprites[LOWORD(trace.m_nRadarSprite)];
                    ImVec2 pos = ImVec2(trace.m_vecPos.x, trace.m_vecPos.y);
                    pos = Util::ConvertMapToScreen(pos, m_fMapSize, screenSz);
                    sprite.Draw(CRect(pos.x-sz, pos.y-sz, pos.x+sz, pos.y+sz), CRGBA(255, 255, 255, 200));
                }
            }
            // create player sprite
            CSprite2d &sprite = CRadar::RadarBlipSprites[RADAR_SPRITE_CENTRE];
            CPlayerPed *pPlayer = FindPlayerPed();
            CVector coord = pPlayer->GetPosition();
            ImVec2 pos = Util::ConvertMapToScreen(ImVec2(coord.x, coord.y), m_fMapSize, screenSz);
            sprite.Draw(CRect(pos.x-sz, pos.y-sz, pos.x+sz, pos.y+sz), CRGBA(255, 255, 255, 200));

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                ImVec2 pos = ImGui::GetMousePos();
                if (pos.x > left && pos.x < right)
                {
                    pos = Util::ConvertScreenToMap(pos, m_fMapSize, screenSz);
                    WarpPlayer<eTeleportType::MapPosition>(CVector(pos.x, pos.y, 0.0f));
                }
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                ImVec2 pos = ImGui::GetMousePos();
                if (pos.x > left && pos.x < right)
                {
                    pos = Util::ConvertScreenToMap(pos, m_fMapSize, screenSz);
                    tRadarTrace *trace = &ms_RadarTrace[LOWORD(FrontEndMenuManager.m_nTargetBlipIndex)];
                    trace->m_vecPos = CVector(pos.x, pos.y, 0.0f);
                    // trace->m_nRadarSprite = RADAR_SPRITE_WAYPOINT;
                }
            }
        }
#endif
    };
}

#ifdef GTASA
template<eTeleportType Type>
void TeleportPage::WarpPlayer(CVector pos, int interiorID)
{
    CPlayerPed* pPlayer = FindPlayerPed();
    CVehicle* pVeh = pPlayer->m_pVehicle;
    int hplayer = CPools::GetPedRef(pPlayer);
    bool jetpack = Command<Commands::IS_PLAYER_USING_JETPACK>(0);

    if (Type == eTeleportType::Marker)
    {
        tRadarTrace targetBlip = ms_RadarTrace[LOWORD(FrontEndMenuManager.m_nTargetBlipIndex)];
        if (targetBlip.m_nRadarSprite != RADAR_SPRITE_WAYPOINT)
        {
            Util::SetMessage(TEXT("Teleport.TargetBlipText"));
            return;
        }
        pos = targetBlip.m_vecPos;
    }

    CStreaming::LoadScene(&pos);
    CStreaming::LoadSceneCollision(&pos);
    CStreaming::LoadAllRequestedModels(false);

    if (Type == eTeleportType::Marker || Type == eTeleportType::MapPosition)
    {
        float ground, water;
        CEntity* pPlayerEntity = FindPlayerEntity(-1);
        ground = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, 1000, nullptr, &pPlayerEntity) + 1.0f;

        if (m_bSpawnUnderwater)
        {
            pos.z = ground;
        }
        else
        {
            Command<Commands::GET_WATER_HEIGHT_AT_COORDS>(pos.x, pos.y, true, &water);
            pos.z = ground > water ? ground : water;
        }

    }

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

            pVeh->m_nAreaCode = interiorID;
        }
    }
    else
    {
        pPlayer->Teleport(pos, false);
    }

    if (jetpack)
    {
        Command<Commands::TASK_JETPACK>(hplayer);
    }

    pPlayer->m_nAreaCode = interiorID;
    Command<Commands::SET_AREA_VISIBLE>(interiorID);
}
#else
template<eTeleportType Type>
void TeleportPage::WarpPlayer(CVector pos, int interiorID)
{
    CPlayerPed* pPlayer = FindPlayerPed();
    CVehicle* pVeh = pPlayer->m_pVehicle;

#ifdef GTAVC
    CStreaming::LoadScene(&pos);
    CStreaming::LoadSceneCollision(&pos);
#else
    CStreaming::LoadScene(pos);
#endif
    CStreaming::LoadAllRequestedModels(false);

    if (pVeh && pPlayer->m_pVehicle && pPlayer->m_bInVehicle)
    {
#ifdef GTAVC
        pVeh->m_nAreaCode = interiorID;
#endif
        pVeh->Teleport(pos);
    }
    else
    {
        pPlayer->Teleport(pos);
    }

#ifdef GTAVC
    pPlayer->m_nAreaCode = interiorID;
    Command<Commands::SET_AREA_VISIBLE>(interiorID);
#endif
}
#endif

void TeleportPage::LocationAddNew()
{
    ImGui::InputTextWithHint(TEXT("Teleport.Location"), TEXT("Teleport.LocationHint"), m_LocBuf, INPUT_BUFFER_SIZE);
    ImGui::InputTextWithHint(TEXT("Teleport.Coordinates"), "x, y, z", m_InBuf, INPUT_BUFFER_SIZE);
    ImGui::Spacing();
    if (ImGui::Button(TEXT("Window.AddEntry"), Widget::CalcSize()))
    {
        std::string key = std::string("Custom.") + m_LocBuf;
        m_locData.m_pData->Set(key.c_str(), ("0, " + std::string(m_InBuf)));
#ifdef GTASA
        // Clear the Radar coordinates
        m_locData.m_pData->RemoveTable("Radar");
#endif

        m_locData.m_pData->Save();
        Util::SetMessage(TEXT("Window.AddEntryMSG"));
    }
}

void TeleportPage::LocationClick(str& unk1, str& unk2, str& loc)
{
    int dim = 0;
    CVector pos;
    if (sscanf(loc.c_str(), "%d,%f,%f,%f", &dim, &pos.x, &pos.y, &pos.z) == 4)
    {
        WarpPlayer<eTeleportType::Coordinate>(pos, dim);
    }
    else
    {
        Util::SetMessage(TEXT("Teleport.InvalidLocation"));
    }
}

void TeleportPage::Draw()
{
    if (ImGui::BeginTabBar("Teleport", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        ImGui::Spacing();
        if (ImGui::BeginTabItem(TEXT( "Window.TeleportPage")))
        {
            ImGui::Spacing();
            if (ImGui::BeginChild("Teleport Child"))
            {
                ImGui::Spacing();
#ifdef GTASA
                ImGui::Columns(2, nullptr, false);
                Widget::Toggle(TEXT("Teleport.InsertCoord"), &m_bInsertCoord);

                if (Widget::Toggle(TEXT("Teleport.QuickTeleport"), &m_bQuickTeleport,
                                     std::string(TEXT_S("Teleport.QuickTeleportHint")
                                                 + quickTeleport.GetNameString()).c_str()))
                {
                    gConfig.Set("Features.QuickTeleport", m_bQuickTeleport);
                }
                ImGui::NextColumn();
                if (Widget::Toggle(TEXT("Teleport.SpawnUnderwater"), &m_bSpawnUnderwater,
                                     TEXT("Teleport.SpawnUnderwaterHint")))
                {
                    gConfig.Set("Features.SpawnUnderwater", m_bSpawnUnderwater);
                }
                if (Widget::Toggle(TEXT("Teleport.TeleportMarker"), &m_bTeleportMarker,
                                     std::string(TEXT_S("Teleport.TeleportMarkerHint")
                                                 + teleportMarker.GetNameString()).c_str()))
                {
                    gConfig.Set("Features.TeleportMarker", m_bTeleportMarker);
                }
                ImGui::Columns(1);
#else
                ImGui::Spacing();
                ImGui::SameLine();
                Widget::Toggle(TEXT("Teleport.InsertCoord"), &m_bInsertCoord);
#endif
                ImGui::Spacing();

                if (m_bInsertCoord)
                {
                    CVector pos = FindPlayerPed()->GetPosition();

                    strcpy(m_InBuf,
                           (std::to_string(static_cast<int>(pos.x)) + ", " + std::to_string(static_cast<int>(pos.y)) +
                            ", " + std::to_string(static_cast<int>(pos.z))).c_str());
                }

                ImGui::InputTextWithHint(TEXT("Teleport.Coordinates"), "x, y, z", m_InBuf, INPUT_BUFFER_SIZE);

                ImGui::Spacing();

                ImVec2 btn_sz = Widget::CalcSize(BY_GAME(3, 2, 2));
                if (ImGui::Button(TEXT("Teleport.TeleportToCoord"), btn_sz))
                {
                    CVector pos{0, 0, 10};
                    if (sscanf(m_InBuf,"%f,%f,%f", &pos.x, &pos.y, &pos.z) == 3)
                    {
                        pos.z += 1.0f;
                        WarpPlayer(pos);
                    }
                    else
                    {
                        Util::SetMessage(TEXT("Teleport.InvalidCoord"));
                    }
                }
                ImGui::SameLine();
#ifdef GTASA
                if (ImGui::Button((TEXT_S("Teleport.TeleportMarker") + "##Btn").c_str(), btn_sz))
                {
                    WarpPlayer<eTeleportType::Marker>();
                }
                ImGui::SameLine();
#endif
                if (ImGui::Button(TEXT("Teleport.TeleportCenter"), btn_sz))
                {
                    WarpPlayer<eTeleportType::Coordinate>(CVector(0, 0, BY_GAME(3, 23, 23)));
                }
                ImGui::Dummy(ImVec2(0, 20));

                if (m_bQuickTeleport)
                {
                    if (ImGui::CollapsingHeader(TEXT("Teleport.CustomMapSize")))
                    {
                        ImGui::TextWrapped(TEXT("Teleport.CustomMapSizeHint"));
                        ImGui::Spacing();
                        if (Widget::InputFloat(TEXT("Teleport.Width"), &m_fMapSize.x, 1.0f, 0.0f, 9999999))
                        {
                            gConfig.Set("Game.MapSizeX", m_fMapSize.x);
                        }
                        if (Widget::InputFloat(TEXT("Teleport.Height"), &m_fMapSize.y, 1.0f, 0.0f, 9999999))
                        {
                            gConfig.Set("Game.MapSizeY", m_fMapSize.y);
                        }
                        ImGui::Spacing();
                        if (ImGui::Button(TEXT("Game.ResetDefault"), Widget::CalcSize()))
                        {
                            m_fMapSize = {6000.0f, 6000.0f};
                            gConfig.Set("Game.MapSizeX", m_fMapSize.x);
                            gConfig.Set("Game.MapSizeY", m_fMapSize.y);
                        }
                        ImGui::Spacing();
                        ImGui::Separator();
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(TEXT( "Window.LocationsTab")))
        {
#ifdef GTASA
            FetchRadarSpriteData();
#endif
            ImGui::Spacing();
            Widget::DataList(m_locData, fArg3Wrapper(teleportPage.LocationClick),
                             fArgNoneWrapper(teleportPage.LocationAddNew));
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
