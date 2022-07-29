#include "pch.h"
#include "menu.h"
#include "widget.h"
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
    Overlay::bCoord = gConfig.Get("Overlay.ShowCoordinates", false);
    Overlay::bCpuUsage = gConfig.Get("Overlay.ShowCPUUsage", false);
    Overlay::bFPS = gConfig.Get("Overlay.ShowFPS", false);
    Overlay::bLocName = gConfig.Get("Overlay.ShowLocationName", false);
    Overlay::bTransparent = gConfig.Get("Overlay.Transparent", false);
    Overlay::bMemUsage = gConfig.Get("Overlay.ShowMemoryUsage", false);
    Overlay::bVehHealth = gConfig.Get("Overlay.ShowVehicleHealth", false);
    Overlay::bVehSpeed = gConfig.Get("Overlay.ShowVehicleSpeed", false);
    Overlay::mSelectedPos = (DisplayPos)gConfig.Get("Overlay.SelectedPosition", (int)DisplayPos::BOTTOM_RIGHT);
    Overlay::fPosX = gConfig.Get("Overlay.PosX", 0);
    Overlay::fPosY = gConfig.Get("Overlay.PosY", 0);
    Overlay::textColor[0] = gConfig.Get("Overlay.TextColor.Red", 1.0f);
    Overlay::textColor[1] = gConfig.Get("Overlay.TextColor.Green", 1.0f);
    Overlay::textColor[2] = gConfig.Get("Overlay.TextColor.Blue", 1.0f);
    Overlay::textColor[3] = gConfig.Get("Overlay.TextColor.Alpha", 1.0f);
    m_bDiscordRPC = gConfig.Get("Menu.DiscordRPC", false);
    m_bAutoCheckUpdate = gConfig.Get("Menu.AutoCheckUpdate", true);
    m_bTextOnlyMode = gConfig.Get("Menu.TextOnlyMode", false);

    Util::GetCPUUsageInit();
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    Overlay::mTotalRam = static_cast<int>(memInfo.ullTotalPhys * 1e-6); // Bytes -> MegaBytes

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
        bool m_bShowMenu = Overlay::bCoord || Overlay::bFPS || Overlay::bLocName || Overlay::bCpuUsage || Overlay::bMemUsage ||
                           ((Overlay::bVehHealth || Overlay::bVehSpeed) && pPlayer && pPlayer->m_pVehicle && pPlayer->m_pVehicle->m_pDriver == pPlayer);

        const float offset = 10.0f;
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        if (Overlay::mSelectedPos == DisplayPos::CUSTOM)
        {
            if (Overlay::fPosX != NULL && Overlay::fPosY != NULL)
            {
                gConfig.Set("Overlay.PosX", Overlay::fPosX);
                gConfig.Set("Overlay.PosY", Overlay::fPosY);
                ImGui::SetNextWindowPos(ImVec2(Overlay::fPosX, Overlay::fPosY), ImGuiCond_Once);
            }
        }
        else
        {
            window_flags |= ImGuiWindowFlags_NoMove;
            ImVec2 pos, pivot;

            if (Overlay::mSelectedPos == DisplayPos::TOP_LEFT)
            {
                pos = ImVec2(offset, offset);
                pivot = ImVec2(0.0f, 0.0f);
            }

            if (Overlay::mSelectedPos == DisplayPos::TOP_RIGHT)
            {
                pos = ImVec2(io.DisplaySize.x - offset, offset);
                pivot = ImVec2(1.0f, 0.0f);
            }

            if (Overlay::mSelectedPos == DisplayPos::BOTTOM_LEFT)
            {
                pos = ImVec2(offset, io.DisplaySize.y - offset);
                pivot = ImVec2(0.0f, 1.0f);
            }

            if (Overlay::mSelectedPos == DisplayPos::BOTTOM_RIGHT)
            {
                pos = ImVec2(io.DisplaySize.x - offset, io.DisplaySize.y - offset);
                pivot = ImVec2(1.0f, 1.0f);
            }

            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, pivot);
        }

        ImGui::SetNextWindowBgAlpha(Overlay::bTransparent ? 0.0f : 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&Overlay::textColor);
        if (m_bShowMenu && ImGui::Begin("Overlay", nullptr, window_flags))
        {
            CVector pos{0,0,0};
            pos = pPlayer->GetPosition();

            size_t game_ms = CTimer::m_snTimeInMilliseconds;
            static size_t interval = 0;
            if (game_ms - interval > 1000)
            {
                Overlay::fCpuUsage = static_cast<float>(Util::GetCurrentCPUUsage());

                MEMORYSTATUSEX memInfo;
                memInfo.dwLength = sizeof(MEMORYSTATUSEX);
                GlobalMemoryStatusEx(&memInfo);
                int mUsedRam = static_cast<int>((memInfo.ullTotalPhys - memInfo.ullAvailPhys) * 1e-6);
                Overlay::fMemUsage = 100.0f * (static_cast<float>(mUsedRam) / static_cast<float>(Overlay::mTotalRam));

                Overlay::mFPS = static_cast<size_t>(BY_GAME(CTimer::game_FPS, io.Framerate, io.Framerate));
                interval = game_ms;
            }

            if (Overlay::bCoord)
            {
                ImGui::Text(TEXT("Menu.Coords"), pos.x, pos.y, pos.z);
            }

            if (Overlay::bCpuUsage)
            {
                ImGui::Text(TEXT("Menu.CPUUsage"), Overlay::fCpuUsage);
            }

            if (Overlay::bFPS)
            {
                ImGui::Text(TEXT("Menu.Frames"), Overlay::mFPS);
            }

            if (Overlay::bLocName)
            {
                ImGui::Text(TEXT("Menu.Location"), Util::GetLocationName(&pos).c_str());
            }

            if (Overlay::bMemUsage)
            {
                ImGui::Text(TEXT("Menu.RAMUsage"), Overlay::fMemUsage);
            }

            if (pPlayer->m_pVehicle && pPlayer->m_pVehicle->m_pDriver == pPlayer)
            {
                if (Overlay::bVehHealth)
                {
                    ImGui::Text((TEXT_S("Menu.VehHealth") + ": %.f").c_str(), pPlayer->m_pVehicle->m_fHealth);
                }

                if (Overlay::bVehSpeed)
                {
                    int speed = pPlayer->m_pVehicle->m_vecMoveSpeed.Magnitude() * 50.0f; // 02E3 - GET_CAR_SPEED
                    ImGui::Text(TEXT("Menu.VehSpeed"), speed);
                }
            }

            ImVec2 windowPos = ImGui::GetWindowPos();
            Overlay::fPosX = windowPos.x;
            Overlay::fPosY = windowPos.y;

            ImGui::End();
        }
        ImGui::PopStyleColor();
    }
}

void Menu::DrawCommandWindow()
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

        if (ImGui::InputTextWithHint("##TEXTFIELD", "Enter command", Commands::m_nInputBuffer, INPUT_BUFFER_SIZE,
                                     ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ProcessCommands();
            Commands::m_bShowMenu = false;
            strcpy(Commands::m_nInputBuffer, "");
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
    std::stringstream ss(Commands::m_nInputBuffer);

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
            SetHelpMessage(TEXT("Menu.InvalidValue"));
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

            Teleport::WarpPlayer(pos);
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
            if (ImGui::Button(TEXT("Menu.ResetSize"), ImVec2(Widget::CalcSize(1))))
            {
                CheatMenu::ResetMenuSize();
            }

            ImGui::Spacing();

            static int selected = Locale::GetCurrentLocaleIndex();
            static std::vector<std::string>& vec = Locale::GetLocaleList();

            if (vec.size() > 0)
            {
                if (Widget::ListBox(TEXT("Menu.Language"), vec, selected))
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
            if (ImGui::Checkbox(TEXT("Menu.AutoCheckUpdate"), &m_bAutoCheckUpdate))
            {
                gConfig.Set("Menu.AutoCheckUpdate", m_bAutoCheckUpdate);
            }
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
            && Widget::Checkbox(TEXT("Menu.TextOnlyMode"), &m_bTextOnlyMode, TEXT("Menu.TextOnlyModeHint")))
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
            if (Widget::ListBox(TEXT("Menu.Position"), Overlay::posNames, (int&)Overlay::mSelectedPos))
            {
                gConfig.Set<int>("Overlay.SelectedPosition", static_cast<int>(Overlay::mSelectedPos));
            }

            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::ColorEdit4(TEXT("Menu.TextColor"), Overlay::textColor))
            {
                gConfig.Set("Overlay.TextColor.Red", Overlay::textColor[0]);
                gConfig.Set("Overlay.TextColor.Green", Overlay::textColor[1]);
                gConfig.Set("Overlay.TextColor.Blue", Overlay::textColor[2]);
                gConfig.Set("Overlay.TextColor.Alpha", Overlay::textColor[3]);
            }

            ImGui::Spacing();

            ImGui::Columns(2, nullptr, false);
            if (ImGui::Checkbox(TEXT("Menu.NoBG"), &Overlay::bTransparent))
            {
                gConfig.Set("Overlay.Transparent", Overlay::bTransparent);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowCoords"), &Overlay::bCoord))
            {
                gConfig.Set("Overlay.ShowCoordinates", Overlay::bCoord);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowCPU"), &Overlay::bCpuUsage))
            {
                gConfig.Set("Overlay.ShowCPUUsage", Overlay::bCpuUsage);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowFPS"), &Overlay::bFPS))
            {
                gConfig.Set("Overlay.ShowFPS", Overlay::bFPS);
            }

            ImGui::NextColumn();

            if (ImGui::Checkbox(TEXT("Menu.ShowLocation"), &Overlay::bLocName))
            {
                gConfig.Set("Overlay.ShowLocationName", Overlay::bLocName);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowRAM"), &Overlay::bMemUsage))
            {
                gConfig.Set("Overlay.ShowMemoryUsage", Overlay::bMemUsage);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowVehHealth"), &Overlay::bVehHealth))
            {
                gConfig.Set("Overlay.ShowVehicleHealth", Overlay::bVehHealth);
            }

            if (ImGui::Checkbox(TEXT("Menu.ShowVehSpeed"), &Overlay::bVehSpeed))
            {
                gConfig.Set("Overlay.ShowVehicleSpeed", Overlay::bVehSpeed);
            }

            ImGui::Columns(1);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Menu.Hotkeys")))
        {
            ImGui::Spacing();
            ImGui::Text(TEXT("Menu.Usage"));
            Widget::Tooltip(TEXT("Menu.UsageText"));
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
            teleportMarker.DrawUI(TEXT("Menu.TPMarkerKey"));
            
            ImGui::Dummy(ImVec2(0, 10));

            fixVeh.DrawUI(TEXT("Menu.FixVehKey"));
            unflipVeh.DrawUI(TEXT("Menu.FlipVehKey"));
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
                if (ImGui::CollapsingHeader(TEXT("Menu.SetArmourCMD")))
                {
                    ImGui::Spacing();
                    ImGui::TextWrapped(TEXT("Menu.SetArmourCMDText"));
                    ImGui::Spacing();
                    ImGui::Separator();
                }
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

            if (ImGui::Button(TEXT("Menu.CheckUpdate"), ImVec2(Widget::CalcSize(3))))
            {
                Updater::CheckUpdate();
            }

            ImGui::SameLine();

            if (ImGui::Button(TEXT("Menu.DiscordServer"), ImVec2(Widget::CalcSize(3))))
            {
                ShellExecute(nullptr, "open", DISCORD_INVITE, nullptr, nullptr, SW_SHOWNORMAL);
            }

            ImGui::SameLine();

            if (ImGui::Button(TEXT("Menu.GitHubRepo"), ImVec2(Widget::CalcSize(3))))
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
                Widget::TextCentered(TEXT("Menu.CopyrightDisclaimer"));

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
