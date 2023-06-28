#include "pch.h"
#include <CSprite.h>
#include "utils/overlay.h"

#ifdef GTASA
#include "pages/teleport.h"
#include "pages/weapon.h"
#include "pages/vehicle.h"
#endif

void Overlay::Init()
{
    m_bCoord = gConfig.Get("Overlay.ShowCoordinates", false);
    m_bCpuUsage = gConfig.Get("Overlay.ShowCPUUsage", false);
    m_bFPS = gConfig.Get("Overlay.ShowFPS", false);
    m_bLocName = gConfig.Get("Overlay.ShowLocationName", false);
    m_bModelInfo = gConfig.Get("Overlay.ShowModelInfo", false);
    m_bPlaytime = gConfig.Get("Overlay.ShowPlaytime", false);
    m_bPedTasks = gConfig.Get("Overlay.ShowPedTasks", false);
    m_bTransparent = gConfig.Get("Overlay.Transparent", false);
    m_bMemUsage = gConfig.Get("Overlay.ShowMemoryUsage", false);
    m_bVehHealth = gConfig.Get("Overlay.ShowVehicleHealth", false);
    m_bVehSpeed = gConfig.Get("Overlay.ShowVehicleSpeed", false);
    m_nSelectedPos = (eDisplayPos)gConfig.Get("Overlay.SelectedPosition", (int)eDisplayPos::BottomRight);
    m_fPos.x = gConfig.Get("Overlay.PosX", 0);
    m_fPos.y = gConfig.Get("Overlay.PosY", 0);
    m_fTextCol[0] = gConfig.Get("Overlay.TextColor.Red", 1.0f);
    m_fTextCol[1] = gConfig.Get("Overlay.TextColor.Green", 1.0f);
    m_fTextCol[2] = gConfig.Get("Overlay.TextColor.Blue", 1.0f);
    m_fTextCol[3] = gConfig.Get("Overlay.TextColor.Alpha", 1.0f);

    Util::GetCPUUsageInit();
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    m_nTotalMem = static_cast<int>(memInfo.ullTotalPhys * 1e-6); // Bytes -> MegaBytes

    ThiscallEvent<AddressList<BY_GAME(0x5343B2, 0x48882E, 0x474BC0), H_CALL>, PRIORITY_BEFORE, ArgPickN<CEntity*, 0>, void(CEntity*)> preRenderEntityEvent;

#ifndef GTASA
    patch::Nop(BY_GAME(NULL, 0x488828, 0x474BBA), 4);
#endif

    // Directly drawing here seems to crash renderer?
    preRenderEntityEvent += [](CEntity *pEnt)
    {
        CPlayerPed *player = FindPlayerPed();
        if (player != pEnt)
        {
            CVector coord = pEnt->GetPosition();
            CVector plaPos = player->GetPosition();

            CColPoint outColPoint;
            if (BY_GAME(pEnt->m_bIsVisible, pEnt->IsVisible(), pEnt->IsVisible()))
            {
                m_EntityList.push_back(pEnt);
            }
#ifdef GTAVC
            if (CModelInfo::GetModelInfo(pEnt->m_nModelIndex)->m_nNum2dEffects > 0)
            {
                pEnt->ProcessLightsForEntity();
            }
#elif GTA3
            // if (CModelInfo::ms_modelInfoPtrs[pEnt->m_nModelIndex]->m_nNum2dEffects > 0)
            // {
            //     pEnt->ProcessLightsForEntity();
            // }
#endif
        }
    };
}

void Overlay::ProcessModelInfo()
{
    if (m_bModelInfo)
    {
        ImDrawList *pDrawList = ImGui::GetWindowDrawList();
        CPlayerPed *player = FindPlayerPed();
        for (CEntity *pEnt : m_EntityList)
        {
            if (pEnt == player)
            {
                continue;
            }

            CVector coord = BY_GAME(,,*)pEnt->GetBoundCentre();
            float distance = DistanceBetweenPoints(coord, player->GetPosition());
            RwV3d screen;
            CVector2D size;
            if (distance < m_fMaxDistance &&
#ifdef GTASA
                    CSprite::CalcScreenCoors(coord.ToRwV3d(), &screen, &size.x, &size.y, true, true)
#else
                    CSprite::CalcScreenCoors(coord.ToRwV3d(), &screen, &size.x, &size.y, true)
#endif
               )
            {
                bool skip = false;
                uint model = pEnt->m_nModelIndex;
                std::string text = std::to_string(model);
                ImU32 col = ImGui::ColorConvertFloat4ToU32(distance < m_fMaxDistance/2 ? ImVec4(1.0f, 1.0f, 1.0f, 1.00f) : ImVec4(0.35f, 0.33f, 0.3f, 1.00f));
#ifdef GTASA
                if (pEnt->m_nType == ENTITY_TYPE_VEHICLE)
                {
                    text = std::format("{}\n{}", model, Util::GetCarName(model));
                }
                else if (pEnt->m_nType == ENTITY_TYPE_PED)
                {
                    CPed *ped = static_cast<CPed*>(pEnt);
                    if (BY_GAME(ped->m_nPedFlags.bInVehicle, ped->m_bInVehicle, ped->m_bInVehicle))
                    {
                        skip = true;
                    }
                }
#endif

                if (!skip)
                {
                    pDrawList->AddText(ImVec2(screen.x, screen.y), col, text.c_str());
                }
            }
        }
    }
}

#ifdef GTASA
void Overlay::ProcessPedTasks()
{
    if (m_bPedTasks)
    {
        ImDrawList *pDrawList = ImGui::GetWindowDrawList();
        CPlayerPed *player = FindPlayerPed();
        for (CEntity *pEnt : m_EntityList)
        {
            if (pEnt == player || pEnt->m_nType != ENTITY_TYPE_PED)
            {
                continue;
            }
            CPed *pPed = static_cast<CPed*>(pEnt);
            CVector coord = BY_GAME(,,*)pPed->GetBoundCentre();
            float distance = DistanceBetweenPoints(coord, player->GetPosition());
            RwV3d screen;
            CVector2D size;
            if (distance < m_fMaxDistance
                    && CSprite::CalcScreenCoors(coord.ToRwV3d(), &screen, &size.x, &size.y, true, true))
            {
                ImU32 col = ImGui::ColorConvertFloat4ToU32(distance < m_fMaxDistance/2 ? ImVec4(1.0f, 1.0f, 1.0f, 1.00f) : ImVec4(0.35f, 0.33f, 0.3f, 1.00f));
                float height = ImGui::GetTextLineHeight();
                screen.y -= 2 * height;

                for (size_t i = 0; i != TASK_SECONDARY_MAX; ++i)
                {
                    CTask *pTask = pPed->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[i];

                    if (pTask)
                    {
                        const char *name = taskNames[pTask->GetId()];
                        pDrawList->AddText(ImVec2(screen.x, screen.y), col, name);
                        screen.y -= height;
                    }
                }
                for (size_t i = 0; i != TASK_PRIMARY_MAX; ++i)
                {
                    CTask *pTask = pPed->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[i];
                    if (pTask)
                    {
                        const char *name = taskNames[pTask->GetId()];
                        pDrawList->AddText(ImVec2(screen.x, screen.y), col, name);
                        screen.y -= height;
                    }
                }
            }
        }
    }
}
#endif

void Overlay::Draw()
{
    if (FrontEndMenuManager.m_bMenuActive)
    {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
                             | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus
                             | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(screen::GetScreenWidth(), screen::GetScreenHeight()));
    ImGui::SetNextWindowBgAlpha(0.0f);
    if (ImGui::Begin("##Overlay", NULL, flags))
    {
        ProcessModelInfo();
#ifdef GTASA
        ProcessPedTasks();
#endif
        ImGui::End();
    }

    ProcessInfoBox();
    ProcessCmdBar();

    // clear the list here
    m_EntityList.clear();
}

void Overlay::ProcessCmdBar()
{
    if (m_bCmdBar)
    {
        int resX = static_cast<int>(screen::GetScreenWidth());
        int resY = static_cast<int>(screen::GetScreenHeight());

        ImGui::SetNextWindowPos(ImVec2(0, resY - 40), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(resX, 40));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration + ImGuiWindowFlags_AlwaysAutoResize +
                                 ImGuiWindowFlags_NoSavedSettings
                                 + ImGuiWindowFlags_NoMove;
        if (ImGui::Begin("CmdBar", nullptr, flags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, resY / 130));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));

            ImGui::SetNextItemWidth(ImGui::GetContentRegionMax().x);

            static char buf[INPUT_BUFFER_SIZE] = "";
            if (ImGui::InputTextWithHint("##TEXTFIELD", "Enter command", buf, INPUT_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                ProcessCommands(std::string(buf));
                m_bCmdBar = false;
                strcpy(buf, "");
            }
            if (!ImGui::IsAnyItemActive())
            {
                ImGui::SetKeyboardFocusHere(-1);
            }
            ImGui::PopStyleVar(2);
            ImGui::End();
        }
    }
}

void Overlay::ProcessCommands(std::string&& str)
{
    std::stringstream ss(str);

    std::string command;
    ss >> command;

    if (command == "armour")
    {
        try
        {
            std::string temp;
            ss >> temp;
            FindPlayerPed()->m_fArmour = std::stof(temp);
        }
        catch (...)
        {
            Util::SetMessage(TEXT("Menu.InvalidValue"));
        }
    }

    if (command == "hp")
    {
        try
        {
            std::string temp;
            ss >> temp;
            FindPlayerPed()->m_fHealth = std::stof(temp);
        }
        catch (...)
        {
            Util::SetMessage(TEXT("Menu.InvalidValue"));
        }
    }

    if (command == "time")
    {
        try
        {
            std::string temp;
            ss >> temp;
            CClock::ms_nGameClockHours = std::stoi(temp);

            ss >> temp;
            CClock::ms_nGameClockMinutes = std::stoi(temp);
        }
        catch (...)
        {
            Util::SetMessage(TEXT("Menu.InvalidValue"));
        }
    }

#ifdef GTASA
    if (command == "tp")
    {
        try
        {
            CVector pos;
            std::string temp;

            ss >> temp;
            pos.x = std::stof(temp);

            ss >> temp;
            pos.y = std::stof(temp);

            ss >> temp;
            pos.z = std::stof(temp);

            teleportPage.WarpPlayer(pos);
        }
        catch (...)
        {
            Util::SetMessage(TEXT("Menu.InvalidLocation"));
        }
    }

    if (command == "wep")
    {
        std::string wep_name;
        ss >> wep_name;

        if (wep_name == "jetpack")
        {
            std::string weapon = "-1";
            weaponPage.GiveWeaponToPlayer(weapon);
            Util::SetMessage(TEXT("Menu.WeaponSpawned"));
        }
        else
        {
            eWeaponType weapon = CWeaponInfo::FindWeaponType((char*)wep_name.c_str());
            std::string weapon_name = std::to_string(weapon);
            CWeaponInfo* pweaponinfo = CWeaponInfo::GetWeaponInfo(weapon, 1);

            if (wep_name != "" && pweaponinfo->m_nModelId1 != -1)
            {
                weaponPage.GiveWeaponToPlayer(weapon_name);
                Util::SetMessage(TEXT("Menu.WeaponSpawned"));
            }
            else
                Util::SetMessage(TEXT("Menu.InvalidComamnd"));
        }

        return;
    }
    if (command == "veh")
    {
        std::string veh_name;
        ss >> veh_name;

        int model = Util::GetCarModel(veh_name.c_str());
        if (model != 0)
        {
            std::string smodel = std::to_string(model);
            vehiclePage.SpawnVehicle(smodel);
            Util::SetMessage(TEXT("Menu.VehicleSpawned"));
        }
        else
            Util::SetMessage(TEXT("Menu.InvalidComamnd"));
    }
#endif
}

void Overlay::ProcessInfoBox()
{
    CPlayerPed* pPlayer = FindPlayerPed();
    if (pPlayer)
    {
        bool m_bShowMenu = m_bCoord || m_bFPS || m_bLocName || m_bCpuUsage || m_bMemUsage ||
                           ((m_bVehHealth || m_bVehSpeed) && pPlayer && pPlayer->m_pVehicle && pPlayer->m_pVehicle->m_pDriver == pPlayer);

        const float offset = 10.0f;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        if (m_nSelectedPos == eDisplayPos::Custom)
        {
            if (m_fPos.x != NULL && m_fPos.y != NULL)
            {
                gConfig.Set("Overlay.PosX", m_fPos.x);
                gConfig.Set("Overlay.PosY", m_fPos.y);
                ImGui::SetNextWindowPos(m_fPos, ImGuiCond_Once);
            }
        }
        else
        {
            window_flags |= ImGuiWindowFlags_NoMove;
            ImVec2 pos, pivot;

            if (m_nSelectedPos == eDisplayPos::TopLeft)
            {
                pos = ImVec2(offset, offset);
                pivot = ImVec2(0.0f, 0.0f);
            }

            if (m_nSelectedPos == eDisplayPos::TopRight)
            {
                pos = ImVec2(io.DisplaySize.x - offset, offset);
                pivot = ImVec2(1.0f, 0.0f);
            }

            if (m_nSelectedPos == eDisplayPos::BottomLeft)
            {
                pos = ImVec2(offset, io.DisplaySize.y - offset);
                pivot = ImVec2(0.0f, 1.0f);
            }

            if (m_nSelectedPos == eDisplayPos::BottomRight)
            {
                pos = ImVec2(io.DisplaySize.x - offset, io.DisplaySize.y - offset);
                pivot = ImVec2(1.0f, 1.0f);
            }

            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
        }

        ImGui::SetNextWindowBgAlpha(m_bTransparent ? 0.0f : 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&m_fTextCol);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        if (m_bShowMenu && ImGui::Begin("Overlay", nullptr, window_flags))
        {
            CVector pos{0,0,0};
            pos = pPlayer->GetPosition();

            size_t game_ms = CTimer::m_snTimeInMilliseconds;
            static size_t interval = 0;
            static float cpuUsage = 0, memUsage = 0;
            if (game_ms - interval > 1000)
            {
                cpuUsage = static_cast<float>(Util::GetCurrentCPUUsage());

                MEMORYSTATUSEX memInfo;
                memInfo.dwLength = sizeof(MEMORYSTATUSEX);
                GlobalMemoryStatusEx(&memInfo);
                int mUsedRam = static_cast<int>((memInfo.ullTotalPhys - memInfo.ullAvailPhys) * 1e-6);
                memUsage = 100.0f * (static_cast<float>(mUsedRam) / static_cast<float>(m_nTotalMem));

                m_nFPS = static_cast<size_t>(BY_GAME(CTimer::game_FPS, io.Framerate, io.Framerate));
                interval = game_ms;
            }

            if (m_bCoord)
            {
                ImGui::Text(TEXT("Menu.Coords"), pos.x, pos.y, pos.z);
            }

            if (m_bCpuUsage)
            {
                ImGui::Text(TEXT("Menu.CPUUsage"), cpuUsage);
            }

            if (m_bFPS)
            {
                ImGui::Text(TEXT("Menu.Frames"), m_nFPS);
            }

            if (m_bLocName)
            {
                ImGui::Text(TEXT("Menu.Location"), Util::GetLocationName(&pos).c_str());
            }

            if (m_bPlaytime)
            {
                int timer = CTimer::m_snTimeInMilliseconds / 1000;
                int h = timer / 3600;
                int m = timer / 60 - h*60;
                int s = timer - m*60;

                if (h == 0)
                {
                    if (m == 0)
                    {
                        ImGui::Text((TEXT_S("Menu.Playtime") + "%d seconds").c_str(), s);
                    }
                    else
                    {
                        ImGui::Text((TEXT_S("Menu.Playtime") + "%d min %d sec").c_str(), m, s);
                    }
                }
                else
                {
                    ImGui::Text((TEXT_S("Menu.Playtime") + "%d hour %d min %d sec").c_str(), h, m, s);
                }
            }

            if (m_bMemUsage)
            {
                ImGui::Text(TEXT("Menu.RAMUsage"), memUsage);
            }

            if (pPlayer->m_pVehicle && pPlayer->m_pVehicle->m_pDriver == pPlayer)
            {
                if (m_bVehHealth)
                {
                    ImGui::Text((TEXT_S("Menu.VehHealth") + ": %.f").c_str(), pPlayer->m_pVehicle->m_fHealth);
                }

                if (m_bVehSpeed)
                {
                    int speed = pPlayer->m_pVehicle->m_vecMoveSpeed.Magnitude() * 50.0f; // 02E3 - GET_CAR_SPEED
                    ImGui::Text(TEXT("Menu.VehSpeed"), speed);
                }
            }

            ImVec2 windowPos = ImGui::GetWindowPos();
            m_fPos = windowPos;

            ImGui::End();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
}