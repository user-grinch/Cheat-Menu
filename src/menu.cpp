#include "pch.h"
#include "menu.h"
#include "ui.h"
#include "util.h"
#include "updater.h"
#include "cheatmenu.h"
#include "rpc.h"

#ifdef GTASA
#include "teleport.h"
#include "weapon.h"
#include "vehicle.h"
#endif

void Menu::Init()
{
    // TODO: use structs
    // Load config data
    m_Overlay::bCoord = gConfig.Get("Overlay.ShowCoordinates", false);
    m_Overlay::bCpuUsage = gConfig.Get("Overlay.ShowCPUUsage", false);
    m_Overlay::bFPS = gConfig.Get("Overlay.ShowFPS", false);
    m_Overlay::bLocName = gConfig.Get("Overlay.ShowLocationName", false);
    m_Overlay::bTransparent = gConfig.Get("Overlay.Transparent", false);
    m_Overlay::bMemUsage = gConfig.Get("Overlay.ShowMemoryUsage", false);
    m_Overlay::bVehHealth = gConfig.Get("Overlay.ShowVehicleName", false);
    m_Overlay::bVehSpeed = gConfig.Get("Overlay.ShowVehicleSpeed", false);
    m_Overlay::mSelectedPos = (DisplayPos)gConfig.Get("Overlay.SelectedPosition", (int)DisplayPos::BOTTOM_RIGHT);
    m_Overlay::fPosX = gConfig.Get("Overlay.PosX", 0);
    m_Overlay::fPosY = gConfig.Get("Overlay.PosY", 0);
    m_Overlay::textColor[0] = gConfig.Get("Overlay.TextColor.Red", 1.0f);
    m_Overlay::textColor[1] = gConfig.Get("Overlay.TextColor.Green", 1.0f);
    m_Overlay::textColor[2] = gConfig.Get("Overlay.TextColor.Blue", 1.0f);
    m_Overlay::textColor[3] = gConfig.Get("Overlay.TextColor.Alpha", 1.0f);
    m_bDiscordRPC = gConfig.Get("Menu.DiscordRPC", false);
    m_bTextOnlyMode = gConfig.Get("Menu.TextOnlyMode", false);

    Util::GetCPUUsageInit();
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    m_Overlay::mTotalRam = static_cast<int>(memInfo.ullTotalPhys * 1e-6); // Bytes -> MegaBytes

    if (m_bDiscordRPC)
    {
        RPC::Init();
    }
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
                gConfig.Set("Overlay.PosX", m_Overlay::fPosX);
                gConfig.Set("Overlay.PosY", m_Overlay::fPosY);
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
                ImGui::Text(TEXT("Menu.Coords"), pos.x, pos.y, pos.z);
            }

            if (m_Overlay::bCpuUsage)
            {
                ImGui::Text(TEXT("Menu.CPUUsage"), m_Overlay::fCpuUsage);
            }

            if (m_Overlay::bFPS)
            {
                ImGui::Text(TEXT("Menu.Frames"), m_Overlay::mFPS);
            }

            if (m_Overlay::bLocName)
            {
                ImGui::Text(TEXT("Menu.Location"), Util::GetLocationName(&pos).c_str());
            }

            if (m_Overlay::bMemUsage)
            {
                ImGui::Text(TEXT("Menu.RAMUsage"), m_Overlay::fMemUsage);
            }

            if (pPlayer->m_pVehicle && pPlayer->m_pVehicle->m_pDriver == pPlayer)
            {
                if (m_Overlay::bVehHealth)
                {
                    ImGui::Text(TEXT("Menu.VehHealth"), pPlayer->m_pVehicle->m_fHealth);
                }

                if (m_Overlay::bVehSpeed)
                {
                    int speed = pPlayer->m_pVehicle->m_vecMoveSpeed.Magnitude() * 50.0f; // 02E3 - GET_CAR_SPEED
                    ImGui::Text(TEXT("Menu.VehSpeed"), speed);
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
            SetHelpMessage(TEXT("Menu.InvalidValue"));
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
            SetHelpMessage(TEXT("Menu.InvalidValue"));
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
            SetHelpMessage(TEXT("Menu.InvalidLocation"));
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
            SetHelpMessage(TEXT("Menu.WeaponSpawned"));
        }
        else
        {
            eWeaponType weapon = CWeaponInfo::FindWeaponType((char*)wep_name.c_str());
            std::string weapon_name = std::to_string(weapon);
            CWeaponInfo* pweaponinfo = CWeaponInfo::GetWeaponInfo(weapon, 1);

            if (wep_name != "" && pweaponinfo->m_nModelId1 != -1)
            {
                Weapon::GiveWeaponToPlayer(weapon_name);
                SetHelpMessage(TEXT("Menu.WeaponSpawned"));
            }
            else
                SetHelpMessage(TEXT("Menu.InvalidComamnd"));
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
            SetHelpMessage(TEXT("Menu.VehicleSpawned"));
        }
        else
            SetHelpMessage(TEXT("Menu.InvalidComamnd"));
    }
#endif
}

void Menu::ShowPage()
{
    if (ImGui::BeginTabBar("Menu", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT("Menu.Config")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Menu.ResetSize"), ImVec2(Ui::GetSize(1))))
            {
                CheatMenu::ResetMenuSize();
            }
            ImGui::Spacing();

            static int selected = Locale::GetCurrentLocaleIndex();
            static std::vector<std::string>& vec = Locale::GetLocaleList();

            if (vec.size() > 0)
            {
                if (Ui::ListBox(TEXT("Menu.Language"), vec, selected))
                {
                    if (Locale::SetLocale(selected) == Locale::eReturnCodes::SUCCESS)
                    {
                        CheatMenu::GenHeaderList();
                    }
                    else
                    {
                        SetHelpMessage(TEXT("Menu.LanguageChangeFailed"));
                    }
                }
            }

            ImGui::Spacing();

            ImGui::Columns(2, NULL, false);
            if (ImGui::Checkbox(TEXT("Menu.DiscordRPC"), &m_bDiscordRPC))
            {
                if (m_bDiscordRPC)
                {
                    RPC::Init();
                }
                else
                {
                    RPC::Shutdown();
                }
                gConfig.Set("Menu.DiscordRPC", m_bDiscordRPC);
            }
            ImGui::NextColumn();

            if (gRenderer == Render_DirectX9
            && Ui::CheckboxWithHint(TEXT("Menu.TextOnlyMode"), &m_bTextOnlyMode, TEXT("Menu.TextOnlyModeHint")))
            {
                gConfig.Set("Menu.TextOnlyMode", m_bTextOnlyMode);
            }
            ImGui::Columns(1);
            
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Menu.Overlay")))
        {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::SameLine();
            if (Ui::ListBox(TEXT("Menu.Position"), m_Overlay::posNames, (int&)m_Overlay::mSelectedPos))
            {
                gConfig.Set<int>("Overlay.SelectedPosition", static_cast<int>(m_Overlay::mSelectedPos));
            }

            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::ColorEdit4(TEXT("Menu.TextColor"), m_Overlay::textColor))
            {
                gConfig.Set("Overlay.TextColor.Red", m_Overlay::textColor[0]);
                gConfig.Set("Overlay.TextColor.Green", m_Overlay::textColor[1]);
                gConfig.Set("Overlay.TextColor.Blue", m_Overlay::textColor[2]);
                gConfig.Set("Overlay.TextColor.Alpha", m_Overlay::textColor[3]);
            }

            ImGui::Spacing();

            ImGui::Columns(2, nullptr, false);
            if (ImGui::Checkbox(TEXT("Menu.NoBG"), &m_Overlay::bTransparent))
            {
                gConfig.Set("Overlay.Transparent", m_Overlay::bTransparent);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowCoords"), &m_Overlay::bCoord))
            {
                gConfig.Set("Overlay.ShowCoordinates", m_Overlay::bCoord);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowCPU"), &m_Overlay::bCpuUsage))
            {
                gConfig.Set("Overlay.ShowCPUUsage", m_Overlay::bCpuUsage);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowFPS"), &m_Overlay::bFPS))
            {
                gConfig.Set("Overlay.ShowFPS", m_Overlay::bFPS);
            }

            ImGui::NextColumn();

            if (ImGui::Checkbox(TEXT("Menu.ShowLocation"), &m_Overlay::bLocName))
            {
                gConfig.Set("Overlay.ShowLocationName", m_Overlay::bLocName);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowRAM"), &m_Overlay::bMemUsage))
            {
                gConfig.Set("Overlay.ShowMemoryUsage", m_Overlay::bMemUsage);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowVehHealth"), &m_Overlay::bVehHealth))
            {
                gConfig.Set("Overlay.ShowVehicleHealth", m_Overlay::bVehHealth);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowVehSpeed"), &m_Overlay::bVehSpeed))
            {
                gConfig.Set("Overlay.ShowVehicleSpeed", m_Overlay::bVehSpeed);
            }

            ImGui::Columns(1);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Menu.Hotkeys")))
        {
            ImGui::Spacing();
            ImGui::Text(TEXT("Menu.Usage"));
            Ui::ShowTooltip(TEXT("Menu.UsageText"));
            ImGui::Spacing();
            ImGui::BeginChild("Hotkeys");
            menuOpen.DrawUI(TEXT("Menu.OpenMenuKey"));
            commandWindow.DrawUI(TEXT("Menu.OpenCMDKey"));

            ImGui::Dummy(ImVec2(0, 10));

            aimSkinChanger.DrawUI(TEXT("Menu.SkinChangerKey"));
            quickSceenShot.DrawUI(TEXT("Menu.QuickSSKey"));
            freeCam.DrawUI(TEXT("Menu.FreecamKey"));
            freeCamForward.DrawUI(TEXT("Menu.FreecamForwardKey"));
            freeCamBackward.DrawUI(TEXT("Menu.FreecamBackwardKey"));
            freeCamLeft.DrawUI(TEXT("Menu.FreecamLeftKey"));
            freeCamRight.DrawUI(TEXT("Menu.FreecamRightKey"));
            quickTeleport.DrawUI(TEXT("Menu.QuickTPKey"));

            ImGui::Dummy(ImVec2(0, 10));

            fixVeh.DrawUI(TEXT("Menu.FixVehKey"));
            flipVeh.DrawUI(TEXT("Menu.FlipVehKey"));
            godMode.DrawUI(TEXT("Menu.GodModeKey"));
            vehEngine.DrawUI(TEXT("Menu.VehEngineKey"));
            vehInstantStart.DrawUI(TEXT("Menu.VehStartKey"));
            vehInstantStop.DrawUI(TEXT("Menu.VehStopKey"));

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Menu.Commands")))
        {
            if (ImGui::BeginChild("CommandsChild"))
            {
                ImGui::TextWrapped(TEXT("Menu.OpenCMDUsing"), commandWindow.GetNameString().c_str());
                ImGui::Spacing();
                if (ImGui::CollapsingHeader(TEXT("Menu.SetHealthCMD")))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped(TEXT("Menu.SetHealthCMDText"));
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader(TEXT("Menu.SetTimeCMD")))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped(TEXT("Menu.SetTimeCMDText"));
                    ImGui::TextWrapped(TEXT("Menu.SetTimeCMDText2"));
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader(TEXT("Menu.TeleportCMD")))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped(TEXT("Menu.TeleportCMDText"));
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader(TEXT("Menu.QuickVehSpawnerCMD")))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped(TEXT("Menu.QuickVehSpawnerCMDText"));
                    ImGui::Spacing();
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader(TEXT("Menu.QuickWepSpawnerCMD")))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped(TEXT("Menu.QuickWepSpawnerCMDText"));
                    ImGui::Spacing();
                    ImGui::Separator();
                }

                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Menu.About")))
        {
            ImGui::Spacing();

            if (ImGui::Button(TEXT("Menu.CheckUpdate"), ImVec2(Ui::GetSize(3))))
            {
                Updater::CheckUpdate();
            }

            ImGui::SameLine();

            if (ImGui::Button(TEXT("Menu.DiscordServer"), ImVec2(Ui::GetSize(3))))
            {
                ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
            }

            ImGui::SameLine();

            if (ImGui::Button(TEXT("Menu.GitHubRepo"), ImVec2(Ui::GetSize(3))))
            {
                ShellExecute(nullptr, "open", GITHUB_LINK, nullptr, nullptr, SW_SHOWNORMAL);
            }
            ImGui::Spacing();

            if (ImGui::BeginChild("AboutChild"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Text("%s: Grinch_", TEXT("Menu.Author"));

                ImGui::Text("%s: %s", TEXT("Menu.Version"), MENU_VERSION);

                ImGui::NextColumn();
                ImGui::Text("ImGui: %s", ImGui::GetVersion());
                ImGui::Text("%s: %s",TEXT("Menu.Build"), BUILD_NUMBER);

                ImGui::Columns(1);

                ImGui::Dummy(ImVec2(0, 10));
                ImGui::TextWrapped(TEXT("Menu.BugDisclaimer"));
                ImGui::Dummy(ImVec2(0, 10));
                Ui::CenterdText(TEXT("Menu.CopyrightDisclaimer"));

                ImGui::Dummy(ImVec2(0, 30));
                if (ImGui::BeginTable("Hall of Fame", 2, ImGuiTableFlags_ScrollY))
                {
                    ImGui::TableSetupColumn(TEXT("Menu.Name"), ImGuiTableColumnFlags_WidthFixed, 100);
                    ImGui::TableSetupColumn(TEXT("Menu.Credits"));
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
                    ImGui::Text("Marzer");
                    ImGui::TableNextColumn();
                    ImGui::Text("TOML parser");

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

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text(TEXT("Main.TranslatorName"));
                    ImGui::TableNextColumn();
                    ImGui::Text(TEXT("Main.Translator"));
                    
                    ImGui::EndTable();
                }

                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}
