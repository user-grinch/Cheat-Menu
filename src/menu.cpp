#include "pch.h"
#include "version.h"
#include "menu.h"
#include "ui.h"
#include "util.h"
#include "updater.h"
#include "cheatmenu.h"

#ifdef GTASA
#include "teleport.h"
#include "weapon.h"
#include "vehicle.h"
#endif

Menu::Menu()
{
    // TODO: use structs
    // Load config data
    m_Overlay::bCoord = gConfig.GetValue("overlay.coord", false);
    m_Overlay::bCpuUsage = gConfig.GetValue("overlay.cpu_usage", false);
    m_Overlay::bFPS = gConfig.GetValue("overlay.fps", false);
    m_Overlay::bLocName = gConfig.GetValue("overlay.loc_name", false);
    m_Overlay::bTransparent = gConfig.GetValue("overlay.transparent", false);
    m_Overlay::bMemUsage = gConfig.GetValue("overlay.mem_usage", false);
    m_Overlay::bVehHealth = gConfig.GetValue("overlay.veh_health", false);
    m_Overlay::bVehSpeed = gConfig.GetValue("overlay.veh_speed", false);
    m_Overlay::mSelectedPos = (DisplayPos)gConfig.GetValue("overlay.selected_pos", (int)DisplayPos::BOTTOM_RIGHT);
    m_Overlay::fPosX = gConfig.GetValue("overlay.pox", 0);
    m_Overlay::fPosY = gConfig.GetValue("overlay.posy", 0);
    m_Overlay::textColor[0] = gConfig.GetValue("overlay.text_color.r", 1.0f);
    m_Overlay::textColor[1] = gConfig.GetValue("overlay.text_color.g", 1.0f);
    m_Overlay::textColor[2] = gConfig.GetValue("overlay.text_color.b", 1.0f);
    m_Overlay::textColor[3] = gConfig.GetValue("overlay.text_color.a", 1.0f);


    // Hotkeys
    aimSkinChanger.m_key1 = gConfig.GetValue("hotkey.aim_skin_changer.key1", VK_RETURN);
    aimSkinChanger.m_key2 = gConfig.GetValue("hotkey.aim_skin_changer.key2", VK_RETURN);

    freeCam.m_key1 = gConfig.GetValue("hotkey.freecam.key1", VK_F6);
    freeCam.m_key2 = gConfig.GetValue("hotkey.freecam.key2", VK_F6);

    quickSceenShot.m_key1 = gConfig.GetValue("hotkey.quick_screenshot.key1", VK_F5);
    quickSceenShot.m_key2 = gConfig.GetValue("hotkey.quick_screenshot.key2", VK_F5);

    quickTeleport.m_key1 = gConfig.GetValue("hotkey.quick_tp.key1", VK_KEY_X);
    quickTeleport.m_key2 = gConfig.GetValue("hotkey.quick_tp.key2", VK_KEY_Y);

    menuOpen.m_key1 = gConfig.GetValue("hotkey.menu_open.key1", VK_LCONTROL);
    menuOpen.m_key2 = gConfig.GetValue("hotkey.menu_open.key2", VK_KEY_M);

    commandWindow.m_key1 = gConfig.GetValue("hotkey.command_window.key1", VK_LMENU);
    commandWindow.m_key2 = gConfig.GetValue("hotkey.command_window.key2", VK_KEY_C);

    flipVeh.m_key1 = gConfig.GetValue("hotkey.flip_veh.key1", VK_NONE);
    flipVeh.m_key2 = gConfig.GetValue("hotkey.flip_veh.key2", VK_NONE);

    fixVeh.m_key1 = gConfig.GetValue("hotkey.fix_veh.key1", VK_NONE);
    fixVeh.m_key2 = gConfig.GetValue("hotkey.fix_veh.key2", VK_NONE);

    godMode.m_key1 = gConfig.GetValue("hotkey.god_mode.key1", VK_NONE);
    godMode.m_key2 = gConfig.GetValue("hotkey.god_mode.key2", VK_NONE);

    vehEngine.m_key1 = gConfig.GetValue("hotkey.veh_engine.key1", VK_NONE);
    vehEngine.m_key2 = gConfig.GetValue("hotkey.veh_engine.key2", VK_NONE);

    vehInstantStart.m_key1 = gConfig.GetValue("hotkey.veh_instant_start.key1", VK_NONE);
    vehInstantStart.m_key2 = gConfig.GetValue("hotkey.veh_instant_start.key2", VK_NONE);

    vehInstantStop.m_key1 = gConfig.GetValue("hotkey.veh_instant_stop.key1", VK_NONE);
    vehInstantStop.m_key2 = gConfig.GetValue("hotkey.veh_instant_stop.key2", VK_NONE);

    Util::GetCPUUsageInit();
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    m_Overlay::mTotalRam = static_cast<int>(memInfo.ullTotalPhys * 1e-6); // Bytes -> MegaBytes
}

void Menu::DrawOverlay()
{
    CPlayerPed* pPlayer = FindPlayerPed();
    if (pPlayer)
    {
        bool m_bShowMenu = m_Overlay::bCoord || m_Overlay::bFPS || m_Overlay::bLocName || m_Overlay::bCpuUsage || m_Overlay::bMemUsage ||
                           ((m_Overlay::bVehHealth || m_Overlay::bVehSpeed) && pPlayer && pPlayer->m_pVehicle && pPlayer->m_pVehicle->m_pDriver == pPlayer);

        const float offset = 10.0f;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        if (m_Overlay::mSelectedPos == DisplayPos::CUSTOM)
        {
            if (m_Overlay::fPosX != NULL && m_Overlay::fPosY != NULL)
            {
                gConfig.SetValue("overlay.posx", m_Overlay::fPosX);
                gConfig.SetValue("overlay.posy", m_Overlay::fPosY);
                ImGui::SetNextWindowPos(ImVec2(m_Overlay::fPosX, m_Overlay::fPosY), ImGuiCond_Once);
            }
        }
        else
        {
            window_flags |= ImGuiWindowFlags_NoMove;
            ImVec2 pos, pivot;

            if (m_Overlay::mSelectedPos == DisplayPos::TOP_LEFT)
            {
                pos = ImVec2(offset, offset);
                pivot = ImVec2(0.0f, 0.0f);
            }

            if (m_Overlay::mSelectedPos == DisplayPos::TOP_RIGHT)
            {
                pos = ImVec2(io.DisplaySize.x - offset, offset);
                pivot = ImVec2(1.0f, 0.0f);
            }

            if (m_Overlay::mSelectedPos == DisplayPos::BOTTOM_LEFT)
            {
                pos = ImVec2(offset, io.DisplaySize.y - offset);
                pivot = ImVec2(0.0f, 1.0f);
            }

            if (m_Overlay::mSelectedPos == DisplayPos::BOTTOM_RIGHT)
            {
                pos = ImVec2(io.DisplaySize.x - offset, io.DisplaySize.y - offset);
                pivot = ImVec2(1.0f, 1.0f);
            }

            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
        }

        ImGui::SetNextWindowBgAlpha(m_Overlay::bTransparent ? 0.0f : 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&m_Overlay::textColor);
        if (m_bShowMenu && ImGui::Begin("Overlay", nullptr, window_flags))
        {
            CVector pos{0,0,0};
            pos = pPlayer->GetPosition();

            size_t game_ms = CTimer::m_snTimeInMilliseconds;
            static size_t interval = 0;
            if (game_ms - interval > 1000)
            {
                m_Overlay::fCpuUsage = static_cast<float>(Util::GetCurrentCPUUsage());

                MEMORYSTATUSEX memInfo;
                memInfo.dwLength = sizeof(MEMORYSTATUSEX);
                GlobalMemoryStatusEx(&memInfo);
                int mUsedRam = static_cast<int>((memInfo.ullTotalPhys - memInfo.ullAvailPhys) * 1e-6);
                m_Overlay::fMemUsage = 100.0f * (static_cast<float>(mUsedRam) / static_cast<float>(m_Overlay::mTotalRam));

                m_Overlay::mFPS = static_cast<size_t>(BY_GAME(CTimer::game_FPS, io.Framerate, io.Framerate));
                interval = game_ms;
            }

            if (m_Overlay::bCoord)
            {
                ImGui::Text("Coord: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
            }

            if (m_Overlay::bCpuUsage)
            {
                ImGui::Text("CPU usage: %.2f%%", m_Overlay::fCpuUsage);
            }

            if (m_Overlay::bFPS)
            {
                ImGui::Text("Frames: %d", m_Overlay::mFPS);
            }

            if (m_Overlay::bLocName)
            {
                ImGui::Text("Location: %s", Util::GetLocationName(&pos).c_str());
            }

            if (m_Overlay::bMemUsage)
            {
                ImGui::Text("RAM usage: %.2f%%", m_Overlay::fMemUsage);
            }

            if (pPlayer->m_pVehicle && pPlayer->m_pVehicle->m_pDriver == pPlayer)
            {
                if (m_Overlay::bVehHealth)
                {
                    ImGui::Text("Veh Health: %.f", pPlayer->m_pVehicle->m_fHealth);
                }

                if (m_Overlay::bVehSpeed)
                {
                    int speed = pPlayer->m_pVehicle->m_vecMoveSpeed.Magnitude() * 50.0f; // 02E3 - GET_CAR_SPEED
                    ImGui::Text("Veh Speed: %d", speed);
                }
            }

            ImVec2 windowPos = ImGui::GetWindowPos();
            m_Overlay::fPosX = windowPos.x;
            m_Overlay::fPosY = windowPos.y;

            ImGui::End();
        }
        ImGui::PopStyleColor();
    }
}

void Menu::DrawShortcutsWindow()
{
    int resX = static_cast<int>(screen::GetScreenWidth());
    int resY = static_cast<int>(screen::GetScreenHeight());

    ImGui::SetNextWindowPos(ImVec2(0, resY - 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(resX, 40));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration + ImGuiWindowFlags_AlwaysAutoResize +
                             ImGuiWindowFlags_NoSavedSettings
                             + ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Shortcuts window", nullptr, flags))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, resY / 130));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));

        ImGui::SetNextItemWidth(ImGui::GetContentRegionMax().x);

        if (ImGui::InputTextWithHint("##TEXTFIELD", "Enter command", m_Commands::m_nInputBuffer, INPUT_BUFFER_SIZE,
                                     ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ProcessCommands();
            m_Commands::m_bShowMenu = false;
            strcpy(m_Commands::m_nInputBuffer, "");
        }
        if (!ImGui::IsAnyItemActive())
        {
            ImGui::SetKeyboardFocusHere(-1);
        }
        ImGui::PopStyleVar(2);
        ImGui::End();
    }
}

void Menu::ProcessCommands()
{
    std::stringstream ss(m_Commands::m_nInputBuffer);

    std::string command;
    ss >> command;

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
            SetHelpMessage("Invalid value", false, false, false);
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
            SetHelpMessage("Invalid value", false, false, false);
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

            Teleport::TeleportPlayer(false, pos, 0);
        }
        catch (...)
        {
            SetHelpMessage("Invalid location", false, false, false);
        }
    }

    if (command == "wep")
    {
        std::string wep_name;
        ss >> wep_name;

        if (wep_name == "jetpack")
        {
            std::string weapon = "-1";
            Weapon::GiveWeaponToPlayer(weapon);
            SetHelpMessage("Weapon given", false, false, false);
        }
        else
        {
            eWeaponType weapon = CWeaponInfo::FindWeaponType((char*)wep_name.c_str());
            std::string weapon_name = std::to_string(weapon);
            CWeaponInfo* pweaponinfo = CWeaponInfo::GetWeaponInfo(weapon, 1);

            if (wep_name != "" && pweaponinfo->m_nModelId1 != -1)
            {
                Weapon::GiveWeaponToPlayer(weapon_name);
                SetHelpMessage("Weapon given", false, false, false);
            }
            else
                SetHelpMessage("Invalid command", false, false, false);
        }

        return;
    }
    if (command == "veh")
    {
        std::string veh_name;
        ss >> veh_name;

        int model = Vehicle::GetModelFromName(veh_name.c_str());
        if (model != 0)
        {
            std::string smodel = std::to_string(model);
            Vehicle::SpawnVehicle(smodel);
            SetHelpMessage("Vehicle spawned", false, false, false);
        }
        else
            SetHelpMessage("Invalid command", false, false, false);
    }
#endif
}

void Menu::ShowPage()
{
    ImGui::Spacing();
    if (ImGui::Button("Reset config", ImVec2(Ui::GetSize(2))))
    {
        gConfig.m_Data.clear();
        SetHelpMessage("Config has been reset. Restart the game for it to take effect.", false, false, false);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset size", ImVec2(Ui::GetSize(2))))
    {
        CheatMenu::ResetMenuSize();
    }

    ImGui::Spacing();
    if (ImGui::BeginTabBar("Menu", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem("Overlay"))
        {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::SameLine();
            if (Ui::ListBox("Position", m_Overlay::posNames, (int&)m_Overlay::mSelectedPos))
            {
                gConfig.SetValue("overlay.selected_pos", m_Overlay::mSelectedPos);
            }

            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::ColorEdit4("Text color", m_Overlay::textColor))
            {
                gConfig.SetValue("overlay.text_color.r", m_Overlay::textColor[0]);
                gConfig.SetValue("overlay.text_color.g", m_Overlay::textColor[1]);
                gConfig.SetValue("overlay.text_color.b", m_Overlay::textColor[2]);
                gConfig.SetValue("overlay.text_color.a", m_Overlay::textColor[3]);
            }

            ImGui::Spacing();

            ImGui::Columns(2, nullptr, false);
            if (ImGui::Checkbox("No background", &m_Overlay::bTransparent))
            {
                gConfig.SetValue("overlay.transparent", m_Overlay::bTransparent);
            }

            if (ImGui::Checkbox("Show coordinates", &m_Overlay::bCoord))
            {
                gConfig.SetValue("overlay.coord", m_Overlay::bCoord);
            }

            if (ImGui::Checkbox("Show CPU usage", &m_Overlay::bCpuUsage))
            {
                gConfig.SetValue("overlay.cpu_usage", m_Overlay::bCpuUsage);
            }

            if (ImGui::Checkbox("Show FPS", &m_Overlay::bFPS))
            {
                gConfig.SetValue("overlay.fps", m_Overlay::bFPS);
            }

            ImGui::NextColumn();

            if (ImGui::Checkbox("Show location", &m_Overlay::bLocName))
            {
                gConfig.SetValue("overlay.loc_name", m_Overlay::bLocName);
            }

            if (ImGui::Checkbox("Show RAM usage", &m_Overlay::bMemUsage))
            {
                gConfig.SetValue("overlay.mem_usage", m_Overlay::bMemUsage);
            }

            if (ImGui::Checkbox("Show veh health", &m_Overlay::bVehHealth))
            {
                gConfig.SetValue("overlay.veh_health", m_Overlay::bVehHealth);
            }

            if (ImGui::Checkbox("Show veh speed", &m_Overlay::bVehSpeed))
            {
                gConfig.SetValue("overlay.veh_speed", m_Overlay::bVehSpeed);
            }

            ImGui::Columns(1);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Hotkeys"))
        {
            ImGui::Spacing();
            ImGui::Text("Usage");
            Ui::ShowTooltip("Left-click selects hotkey.\nLeft clicking outside deselects."
                            "\nRight click disables hotkey.");
            ImGui::Spacing();
            ImGui::BeginChild("Hotkeys");
            if (menuOpen.DrawUI("Open/ close cheat menu"))
            {
                gConfig.SetValue("hotkey.menu_open.key1", menuOpen.m_key1);
                gConfig.SetValue("hotkey.menu_open.key2", menuOpen.m_key2);
            }
            if (commandWindow.DrawUI("Open/ close command window"))
            {
                gConfig.SetValue("hotkey.command_window.key1", commandWindow.m_key1);
                gConfig.SetValue("hotkey.command_window.key2", commandWindow.m_key2);
            }

            ImGui::Dummy(ImVec2(0, 10));

            if (aimSkinChanger.DrawUI("Activate aim skin changer"))
            {
                gConfig.SetValue("hotkey.aim_skin_changer.key1", aimSkinChanger.m_key1);
                gConfig.SetValue("hotkey.aim_skin_changer.key2", aimSkinChanger.m_key2);
            }
            if (freeCam.DrawUI("Freecam"))
            {
                gConfig.SetValue("hotkey.freecam.key1", freeCam.m_key1);
                gConfig.SetValue("hotkey.freecam.key2", freeCam.m_key2);
            }
            if (quickSceenShot.DrawUI("Take quick screenshot"))
            {
                gConfig.SetValue("hotkey.quick_screenshot.key1", quickSceenShot.m_key1);
                gConfig.SetValue("hotkey.quick_screenshot.key2", quickSceenShot.m_key2);
            }
            if (quickTeleport.DrawUI("Toggle quick teleport"))
            {
                gConfig.SetValue("hotkey.quick_tp.key1", quickTeleport.m_key1);
                gConfig.SetValue("hotkey.quick_tp.key2", quickTeleport.m_key2);
            }

            ImGui::Dummy(ImVec2(0, 10));

            if (fixVeh.DrawUI("Fix current vehicle"))
            {
                gConfig.SetValue("hotkey.fix_veh.key1", fixVeh.m_key1);
                gConfig.SetValue("hotkey.fix_veh.key2", fixVeh.m_key2);
            }

            if (flipVeh.DrawUI("Flip current vehicle"))
            {
                gConfig.SetValue("hotkey.flip_veh.key1", flipVeh.m_key1);
                gConfig.SetValue("hotkey.flip_veh.key2", flipVeh.m_key2);
            }

            if (godMode.DrawUI("Toggle god mode"))
            {
                gConfig.SetValue("hotkey.god_mode.key1", godMode.m_key1);
                gConfig.SetValue("hotkey.god_mode.key2", godMode.m_key2);
            }

            if (vehEngine.DrawUI("Toggle veh engine"))
            {
                gConfig.SetValue("hotkey.veh_engine.key1", vehEngine.m_key1);
                gConfig.SetValue("hotkey.veh_engine.key2", vehEngine.m_key2);
            }

            if (vehInstantStart.DrawUI("Vehicle instant start"))
            {
                gConfig.SetValue("hotkey.veh_instant_start.key1", vehInstantStart.m_key1);
                gConfig.SetValue("hotkey.veh_instant_start.key2", vehInstantStart.m_key2);
            }

            if (vehInstantStop.DrawUI("Vehicle instant stop"))
            {
                gConfig.SetValue("hotkey.veh_instant_stop.key1", vehInstantStop.m_key1);
                gConfig.SetValue("hotkey.veh_instant_stop.key2", vehInstantStop.m_key2);
            }

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Commands"))
        {
            if (ImGui::BeginChild("CommandsChild"))
            {
                ImGui::TextWrapped("Open or close command window using %s", commandWindow.GetNameString().c_str());
                ImGui::Spacing();
                if (ImGui::CollapsingHeader("Set health"))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped("Set player health.\nExample: hp (health).");
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Set time"))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped("Set current game time.\nExample: time (hour) (minute).\n");
                    ImGui::TextWrapped("Writing something like 'time 12' would be interpreted as 'time 12 12'");
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Teleport"))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped("Teleports player to specified coordinates.\nExample: tp x y z");
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Quick vehicle spawner"))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped("Spawn vehicles by typing their model names.\nExample: veh (veh_name)");
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Quick weapon spawner"))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped("Spawn weapons by typing their model names.\nExample: wep (wep_name)");
                    ImGui::Spacing();
                    ImGui::Separator();
                }

                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("About"))
        {
            ImGui::Spacing();

            if (ImGui::Button("Check update", ImVec2(Ui::GetSize(3))))
            {
                Updater::CheckUpdate();
            }

            ImGui::SameLine();

            if (ImGui::Button("Discord server", ImVec2(Ui::GetSize(3))))
            {
                ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
            }

            ImGui::SameLine();

            if (ImGui::Button("GitHub repo", ImVec2(Ui::GetSize(3))))
            {
                ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
            }
            ImGui::Spacing();

            if (ImGui::BeginChild("AboutChild"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Text("Author: Grinch_");

                ImGui::Text("Version: %s",MENU_VERSION);

                ImGui::NextColumn();
                ImGui::Text("ImGui: %s", ImGui::GetVersion());
                ImGui::Text("Build: %s", BUILD_NUMBER);

                ImGui::Columns(1);

                ImGui::Dummy(ImVec2(0, 10));
                ImGui::TextWrapped("If you find bugs or have suggestions, let me know on discord.");
                ImGui::Dummy(ImVec2(0, 10));
                Ui::CenterdText("Copyright Grinch_ 2019-2022. All rights reserved");

                ImGui::Dummy(ImVec2(0, 30));
                if (ImGui::BeginTable("Hall of Fame", 2, ImGuiTableFlags_ScrollY))
                {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 100);
                    ImGui::TableSetupColumn("Credits");
                    ImGui::TableHeadersRow();
                    
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Codenulls");
                    ImGui::TableNextColumn();
                    ImGui::Text("III & VC animation code");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("DKPac22");
                    ImGui::TableNextColumn();
                    ImGui::Text("Plugin SDK, vehicle texture code");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Junior-Djjr");
                    ImGui::TableNextColumn();
                    ImGui::Text("Code & suggestions");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Ocornut");
                    ImGui::TableNextColumn();
                    ImGui::Text("ImGui");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Rebzzel");
                    ImGui::TableNextColumn();
                    ImGui::Text("Keiro");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("TsudaKageyu");
                    ImGui::TableNextColumn();
                    ImGui::Text("MinHook");
                    
                    ImGui::EndTable();
                }

                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
