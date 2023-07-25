#include "pch.h"
#include "menu.h"
#include "cheatmenu.h"
#include "utils/widget.h"
#include "utils/util.h"
#include "utils/updater.h"
#include "utils/rpc.h"
#include "utils/overlay.h"

MenuPage& menuPage = MenuPage::Get();
MenuPage::MenuPage()
    : IPage<MenuPage>(ePageID::Menu, "Settings", true)
{
    // This needs to run before initRwEvent
    // Updates are checked in m_bAutoCheckUpdate
    Events::initRwEvent.before += [this]()
    {
        m_bDiscordRPC = gConfig.Get("Menu.DiscordRPC", false);
        m_bAutoCheckUpdate = gConfig.Get("Menu.AutoCheckUpdate", true);
        m_bTextOnlyMode = gConfig.Get("Menu.TextOnlyMode", false);

        m_fAccentColor[0] = gConfig.Get("Menu.AccentColor.Red", 0.0392f);
        m_fAccentColor[1] = gConfig.Get("Menu.AccentColor.Green", 0.2784f);
        m_fAccentColor[2] = gConfig.Get("Menu.AccentColor.Blue", 0.6078f);
        CheatMenu.UpdateAccentColor(m_fAccentColor);
    };

    if (m_bDiscordRPC)
    {
        RPC::Init();
    }
}

void MenuPage::Draw()
{
    if (ImGui::BeginTabBar("Menu", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::BeginTabItem(TEXT( "Menu.Config")))
        {
            static int selected = Locale::GetCurrentLocaleIndex();
            static std::vector<std::string>& vec = Locale::GetLocaleList();

            ImGui::Spacing();
            if (ImGui::Button(TEXT("Menu.ResetSize"), ImVec2(Widget::CalcSize(1))))
            {
                CheatMenu.ResetParams();
            }
            // ImGui::SameLine();
            // if (ImGui::Button(TEXT("Menu.ReloadFonts"), ImVec2(Widget::CalcSize(2))))
            // {
            //     FontMgr::SetFontReloadRequired(true);
            // }

            ImGui::Spacing();

            if (vec.size() > 0)
            {
                ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 3);
                if (Widget::ListBox(TEXT("Menu.Language"), vec, selected))
                {
                    if (Locale::SetLocale(selected) == Locale::eReturnCodes::SUCCESS)
                    {
                        std::string label = vec[selected];
                        gConfig.Set("Menu.Language", label);
                    }
                    else
                    {
                        Util::SetMessage(TEXT("Menu.LanguageChangeFailed"));
                    }
                }

                if (ImGui::ColorEdit3(TEXT("Menu.SelectAccentColor"), m_fAccentColor))
                {
                    gConfig.Set("Menu.AccentColor.Red", m_fAccentColor[0]);
                    gConfig.Set("Menu.AccentColor.Green", m_fAccentColor[1]);
                    gConfig.Set("Menu.AccentColor.Blue", m_fAccentColor[2]);
                    CheatMenu.UpdateAccentColor(m_fAccentColor);
                }

                ImGui::PopItemWidth();
            }

            ImGui::Dummy(ImVec2(0, 20));

            ImGui::Columns(2, NULL, false);
            if (Widget::Toggle(TEXT("Menu.AutoCheckUpdate"), &m_bAutoCheckUpdate))
            {
                gConfig.Set("Menu.AutoCheckUpdate", m_bAutoCheckUpdate);
            }
            if (Widget::Toggle(TEXT("Menu.DiscordRPC"), &m_bDiscordRPC))
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

            if (gRenderer == eRenderer::DirectX9
                    && Widget::Toggle(TEXT("Menu.TextOnlyMode"), &m_bTextOnlyMode, TEXT("Menu.TextOnlyModeHint")))
            {
                gConfig.Set("Menu.TextOnlyMode", m_bTextOnlyMode);
            }
            ImGui::Columns(1);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Menu.Overlay")))
        {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::Combo(TEXT("Menu.Position"), reinterpret_cast<int*>(&Overlay::m_nSelectedPos),
                             "Custom\0Top left\0Top right\0Bottom left\0Bottom right\0"))
            {
                gConfig.Set<int>("Overlay.SelectedPosition", static_cast<int>(Overlay::m_nSelectedPos));
            }

            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::ColorEdit4(TEXT("Menu.TextColor"), Overlay::m_fTextCol))
            {
                gConfig.Set("Overlay.TextColor.Red", Overlay::m_fTextCol[0]);
                gConfig.Set("Overlay.TextColor.Green", Overlay::m_fTextCol[1]);
                gConfig.Set("Overlay.TextColor.Blue", Overlay::m_fTextCol[2]);
                gConfig.Set("Overlay.TextColor.Alpha", Overlay::m_fTextCol[3]);
            }

            ImGui::Dummy(ImVec2(0, 20));

            ImGui::Columns(2, nullptr, false);
            if (Widget::Toggle(TEXT("Menu.NoBG"), &Overlay::m_bTransparent))
            {
                gConfig.Set("Overlay.Transparent", Overlay::m_bTransparent);
            }

            if (Widget::Toggle(TEXT("Menu.ShowCoords"), &Overlay::m_bCoord))
            {
                gConfig.Set("Overlay.ShowCoordinates", Overlay::m_bCoord);
            }

            if (Widget::Toggle(TEXT("Menu.ShowCPU"), &Overlay::m_bCpuUsage))
            {
                gConfig.Set("Overlay.ShowCPUUsage", Overlay::m_bCpuUsage);
            }

            if (Widget::Toggle(TEXT("Menu.ShowFPS"), &Overlay::m_bFPS))
            {
                gConfig.Set("Overlay.ShowFPS", Overlay::m_bFPS);
            }

            if (Widget::Toggle(TEXT("Menu.ShowLocation"), &Overlay::m_bLocName))
            {
                gConfig.Set("Overlay.ShowLocationName", Overlay::m_bLocName);
            }

#ifndef GTA3
            if (Widget::Toggle(TEXT("Menu.ShowModelInfo"), &Overlay::m_bModelInfo))
            {
                gConfig.Set("Overlay.ShowModelInfo", Overlay::m_bModelInfo);
            }
#endif

            ImGui::NextColumn();

            if (Widget::Toggle(TEXT("Menu.ShowPlaytime"), &Overlay::m_bPlaytime))
            {
                gConfig.Set("Overlay.ShowPlaytime", Overlay::m_bPlaytime);
            }

            if (Widget::Toggle(TEXT("Menu.ShowPedTasks"), &Overlay::m_bPedTasks))
            {
                gConfig.Set("Overlay.ShowPedTasks", Overlay::m_bPedTasks);
            }

            if (Widget::Toggle(TEXT("Menu.ShowRAM"), &Overlay::m_bMemUsage))
            {
                gConfig.Set("Overlay.ShowMemoryUsage", Overlay::m_bMemUsage);
            }

            if (Widget::Toggle(TEXT("Menu.ShowVehHealth"), &Overlay::m_bVehHealth))
            {
                gConfig.Set("Overlay.ShowVehicleHealth", Overlay::m_bVehHealth);
            }

            if (Widget::Toggle(TEXT("Menu.ShowVehSpeed"), &Overlay::m_bVehSpeed))
            {
                gConfig.Set("Overlay.ShowVehicleSpeed", Overlay::m_bVehSpeed);
            }

            ImGui::Columns(1);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Menu.Hotkeys")))
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
        if (ImGui::BeginTabItem(TEXT( "Menu.Commands")))
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
        if (ImGui::BeginTabItem(TEXT( "Menu.About")))
        {
            ImGui::Spacing();

            ImVec2 btn_sz = ImVec2(Widget::CalcSize(4));
            if (ImGui::Button(TEXT("Menu.CheckUpdate"), btn_sz))
            {
                Updater::CheckUpdate();
            }

            ImGui::SameLine();

            if (ImGui::Button(TEXT("Menu.DiscordServer"), btn_sz))
            {
                OPEN_LINK(DISCORD_INVITE);
            }

            ImGui::SameLine();
            
            if (ImGui::Button(TEXT("Menu.GitHubRepo"), btn_sz))
            {
                OPEN_LINK(GITHUB_LINK);
            }

            ImGui::SameLine();

            if (ImGui::Button(TEXT("Menu.Patreon"), btn_sz))
            {
                OPEN_LINK(PATREON_LINK);
            }
            ImGui::Spacing();

            if (ImGui::BeginChild("AboutChild"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Text("%s: Grinch_", TEXT("Menu.Author"));

                ImGui::Text("%s: %s", TEXT("Menu.Version"), MENU_VERSION);

                ImGui::NextColumn();
                ImGui::Text("ImGui: %s", ImGui::GetVersion());
                ImGui::Text("%s: %s",TEXT("Menu.Build"), __DATE__);

                ImGui::Columns(1);

                ImGui::Dummy(ImVec2(0, 10));
                ImGui::TextWrapped(TEXT("Menu.BugDisclaimer"));
                ImGui::Dummy(ImVec2(0, 10));
                Widget::TextCentered(TEXT("Menu.PatreonText"));
                Widget::TextCentered(TEXT("Menu.CopyrightDisclaimer"));

                ImGui::Dummy(ImVec2(0, 30));
                if (ImGui::BeginTable("Hall of Fame", 2, NULL))
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
                    ImGui::Text("cmdwtf");
                    ImGui::TableNextColumn();
                    ImGui::Text("ImGui toggles");

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
                    ImGui::Text("Seeman");
                    ImGui::TableNextColumn();
                    ImGui::Text("Installer");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("TsudaKageyu");
                    ImGui::TableNextColumn();
                    ImGui::Text("MinHook");

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("_AG & jeremii");
                    ImGui::TableNextColumn();
                    ImGui::Text("Radar code");

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
