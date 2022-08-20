#include "pch.h"
#include "vehpaint.h"
#include "utils/widget.h"
#include "custom/filehandler.h"

#ifdef GTASA
#include "custom/neon_sa.h"
#include "custom/paint_sa.h"
#endif

VehPaintMgr& VehPaint = VehPaintMgr::Get();

VehPaintMgr::VehPaintMgr()
{
    FileHandler::FetchColorData(m_ColorData);

#ifdef GTASA
    Paint::InjectHooks();

    Events::processScriptsEvent += [this]
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        CPlayerPed* pPlayer = FindPlayerPed();
        CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());

        if (pPlayer && Util::IsInCar())
        {
            if (m_Neon.m_bRainbowEffect && timer - m_Neon.m_nRainbowTimer > 50)
            {
                int red, green, blue;

                Util::RainbowValues(red, green, blue, 0.25);
                Neon.Install(pVeh, red, green, blue);
                m_Neon.m_nRainbowTimer = timer;
            }
        }

        // Traffic neons
        if (m_Neon.m_bApplyOnTraffic && timer - m_Neon.m_nTrafficTimer > 1000)
        {
            for (CVehicle* veh : CPools::ms_pVehiclePool)
            {
                int chance = 0;

                if (veh->m_nVehicleClass == CLASS_NORMAL) // Normal
                {
                    chance = Random(1, 20);
                }

                if (veh->m_nVehicleClass == CLASS_RICHFAMILY) // Rich family
                {
                    chance = Random(1, 4);
                }

                if (veh->m_nVehicleClass == CLASS_EXECUTIVE) // Executive
                {
                    chance = Random(1, 3);
                }

                if (chance == 1 && !Neon.IsInstalled(veh) && veh->m_pDriver != pPlayer)
                {
                    Neon.Install(veh, Random(0, 255), Random(0, 255), Random(0, 255));
                }
            }
            m_Neon.m_nTrafficTimer = timer;
        }
    };
#endif
}

void VehPaintMgr::Draw()
{
    CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());
    int hVeh = CPools::GetVehicleRef(pVeh);
    
    if (ImGui::BeginTabItem(TEXT("Vehicle.Color")))
    {
#ifdef GTASA
        Paint::GenerateNodeList(pVeh, m_Paint.m_vecNames, m_Paint.m_Selected);

        ImGui::Spacing();
        if (ImGui::Button(TEXT("Vehicle.ResetColor"), ImVec2(Widget::CalcSize())))
        {
            Paint::ResetNodeColor(pVeh, m_Paint.m_Selected);
            Util::SetMessage(TEXT("Vehicle.ResetColorMSG"));
        }
        ImGui::Spacing();

        Widget::ListBox(TEXT("Vehicle.Component"), m_Paint.m_vecNames, m_Paint.m_Selected);

        if (ImGui::ColorEdit3(TEXT("Vehicle.ColorPicker"), m_Paint.m_fColorPicker))
        {
            uchar r = m_Paint.m_fColorPicker[0] * 255;
            uchar g = m_Paint.m_fColorPicker[1] * 255;
            uchar b = m_Paint.m_fColorPicker[2] * 255;
            Paint::SetNodeColor(pVeh, m_Paint.m_Selected, { r, g, b, 255 }, m_Paint.m_bMatFilter);
        }
#endif

        ImGui::Spacing();
        ImGui::Columns(2, NULL, false);

#ifdef GTASA
        ImGui::Checkbox(TEXT("Vehicle.MatFilter"), &m_Paint.m_bMatFilter);
        ImGui::RadioButton(TEXT("Vehicle.Primary"), &m_Paint.m_nRadioButton, 1);
        ImGui::RadioButton(TEXT("Vehicle.Secondary"), &m_Paint.m_nRadioButton, 2);
        ImGui::NextColumn();
        ImGui::NewLine();
        ImGui::RadioButton(TEXT("Vehicle.Tertiary"), &m_Paint.m_nRadioButton, 3);
        ImGui::RadioButton(TEXT("Vehicle.Quaternary"), &m_Paint.m_nRadioButton, 4);
#else
        ImGui::RadioButton(TEXT("Vehicle.Primary"), &m_Paint.m_nRadioButton, 1);
        ImGui::NextColumn();
        ImGui::RadioButton(TEXT("Vehicle.Secondary"), &m_Paint.m_nRadioButton, 2);
#endif
        ImGui::Spacing();
        ImGui::Columns(1);
        ImGui::Text(TEXT("Vehicle.SelectPreset"));
        ImGui::Spacing();

        int count = (int)m_ColorData.size();

        ImVec2 size = Widget::CalcSize();
        int btnsInRow = ImGui::GetWindowContentRegionWidth() / (size.y * 2);
        int btnSize = (ImGui::GetWindowContentRegionWidth() - int(ImGuiStyleVar_ItemSpacing) * (btnsInRow -
                        0.6 * btnsInRow)) / btnsInRow;

        ImGui::BeginChild("Colorss");

        for (int colorId = 0; colorId < count; ++colorId)
        {
            if (Widget::ColorBtn(colorId, m_ColorData[colorId], ImVec2(btnSize, btnSize)))
            {
                *(uint8_replacement*)(int(pVeh) + BY_GAME(0x433, 0x19F, 0x19B) + m_Paint.m_nRadioButton) = colorId;
            }

            if ((colorId + 1) % btnsInRow != 0)
            {
                ImGui::SameLine(0.0, 4.0);
            }
        }

        ImGui::EndChild();
        ImGui::EndTabItem();
    }

#ifdef GTASA
    if (gRenderer != eRenderer::DirectX11)
    {
        CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());

        if (ImGui::BeginTabItem(TEXT("Vehicle.NeonsTab")))
        {
            int model = pVeh->m_nModelIndex;
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Vehicle.RemoveNeon"), ImVec2(Widget::CalcSize())))
            {
                Neon.Remove(pVeh);
                Util::SetMessage(TEXT("Vehicle.RemoveNeonMSG"));
            }

            ImGui::Spacing();
            ImGui::Columns(2, NULL, false);

            bool pulsing = Neon.IsPulsingEnabled(pVeh);
            if (Widget::Checkbox(TEXT("Vehicle.PulsingNeon"), &pulsing))
            {
                Neon.SetPulsing(pVeh, pulsing);
            }

            Widget::Checkbox(TEXT("Vehicle.RainbowNeon"), &m_Neon.m_bRainbowEffect, TEXT("Vehicle.RainbowNeonMSG"));
            ImGui::NextColumn();
            Widget::Checkbox(TEXT("Vehicle.TrafficNeon"), &m_Neon.m_bApplyOnTraffic, TEXT("Vehicle.TrafficNeonMSG"));
            ImGui::Columns(1);

            ImGui::Spacing();

            if (ImGui::ColorEdit3(TEXT("Vehicle.ColorPicker"), m_Neon.m_fColorPicker))
            {
                int r = static_cast<int>(m_Neon.m_fColorPicker[0] * 255);
                int g = static_cast<int>(m_Neon.m_fColorPicker[1] * 255);
                int b = static_cast<int>(m_Neon.m_fColorPicker[2] * 255);

                Neon.Install(pVeh, r, g, b);
            }


            ImGui::Spacing();
            ImGui::Text(TEXT("Vehicle.SelectPreset"));

            int count = (int)m_ColorData.size();
            ImVec2 size = Widget::CalcSize();
            int btnsInRow = ImGui::GetWindowContentRegionWidth() / (size.y * 2);
            int btnSize = (ImGui::GetWindowContentRegionWidth() - int(ImGuiStyleVar_ItemSpacing) * (btnsInRow -
                            0.6 * btnsInRow)) / btnsInRow;

            ImGui::BeginChild("Neonss");

            for (int color_id = 0; color_id < count; ++color_id)
            {
                auto& color = m_ColorData[color_id];
                if (Widget::ColorBtn(color_id, color, ImVec2(btnSize, btnSize)))
                {
                    int r = static_cast<int>(color[0] * 255);
                    int g = static_cast<int>(color[1] * 255);
                    int b = static_cast<int>(color[2] * 255);

                    Neon.Install(pVeh, r, g, b);
                }

                if ((color_id + 1) % btnsInRow != 0)
                {
                    ImGui::SameLine(0.0, 4.0);
                }
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(TEXT("Vehicle.TextureTab")))
        {
            Paint::GenerateNodeList(pVeh, m_Paint.m_vecNames, m_Paint.m_Selected);

            ImGui::Spacing();
            if (ImGui::Button(TEXT("Vehicle.ResetTexture"), ImVec2(Widget::CalcSize())))
            {
                Paint::ResetNodeTexture(pVeh, m_Paint.m_Selected);
                Util::SetMessage(TEXT("Vehicle.ResetTextureMSG"));
            }
            ImGui::Spacing();

            Widget::ListBox(TEXT("Vehicle.Component"), m_Paint.m_vecNames, m_Paint.m_Selected);
            ImGui::Spacing();

            ImGui::Columns(2, NULL, false);
            ImGui::Checkbox(TEXT("Vehicle.MatFilter"), &m_Paint.m_bMatFilter);
            ImGui::NextColumn();
            int maxpjob, curpjob;
            Command<Commands::GET_NUM_AVAILABLE_PAINTJOBS>(hVeh, &maxpjob);

            if (maxpjob > 0)
            {
                Command<Commands::GET_CURRENT_VEHICLE_PAINTJOB>(hVeh, &curpjob);
                
                if (ImGui::ArrowButton("Left", ImGuiDir_Left))
                {
                    curpjob -= 1;
                    if (curpjob < -1)
                    {
                        curpjob = maxpjob - 1; 
                    }
                    Command<Commands::GIVE_VEHICLE_PAINTJOB>(hVeh, curpjob);
                }
                ImGui::SameLine();
                ImGui::Text("%s: %d",TEXT("Vehicle.Paintjob"), curpjob+2);
                ImGui::SameLine();
                if (ImGui::ArrowButton("Right", ImGuiDir_Right))
                {
                    curpjob += 1;
                    if (curpjob > maxpjob)
                    {
                        curpjob =  -1; 
                    }
                    Command<Commands::GIVE_VEHICLE_PAINTJOB>(hVeh, curpjob);
                }

                ImGui::Spacing();
            }
            ImGui::Columns(1);
            ImGui::Spacing();
            Widget::ImageList(Paint::m_TextureData,
            [this](std::string& str)
            {
                Paint::SetNodeTexture(FindPlayerPed()->m_pVehicle, m_Paint.m_Selected, str,
                                        m_Paint.m_bMatFilter);
            },
            [](std::string& str)
            {
                return str;
            }, nullptr, [](){});

            ImGui::EndTabItem();
        }
    }
#endif
}