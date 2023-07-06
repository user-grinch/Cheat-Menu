#include "pch.h"
#include "vehcustmzr.h"
#include "utils/widget.h"
#include "custom/filehandler.h"

#ifdef GTASA
#include "custom/neon_sa.h"
#include "custom/paint_sa.h"
#endif

static std::vector<std::string> m_HandlingFlagNames = // 32 flags
{
    "1G_BOOST", "2G_BOOST", "NPC_ANTI_ROLL", "NPC_NEUTRAL_HANDL", "NO_HANDBRAKE", "STEER_REARWHEELS",
    "HB_REARWHEEL_STEER", "ALT_STEER_OPT",
    "WHEEL_F_NARROW2", "WHEEL_F_NARROW", "WHEEL_F_WIDE", "WHEEL_F_WIDE2", "WHEEL_R_NARROW2", "WHEEL_R_NARROW",
    "WHEEL_R_WIDE", "WHEEL_R_WIDE2",
    "HYDRAULIC_GEOM", "HYDRAULIC_INST", "HYDRAULIC_NONE", "NOS_INST", "OFFROAD_ABILITY", "OFFROAD_ABILITY2",
    "HALOGEN_LIGHTS", "PROC_REARWHEEL_1ST",
    "USE_MAXSP_LIMIT", "LOW_RIDER", "STREET_RACER", "SWINGING_CHASSIS", "Unused 1", "Unused 2", "Unused 3",
    "Unused 4"
};

static std::vector<std::string> m_ModelFlagNames = // 32 flags
{
    "IS_VAN", "IS_BUS", "IS_LOW", "IS_BIG", "REVERSE_BONNET", "HANGING_BOOT", "TAILGATE_BOOT", "NOSWING_BOOT",
    "NO_DOORS", "TANDEM_SEATS",
    "SIT_IN_BOAT", "CONVERTIBLE", "NO_EXHAUST", "DOUBLE_EXHAUST", "NO1FPS_LOOK_BEHIND", "FORCE_DOOR_CHECK",
    "AXLE_F_NOTILT", "AXLE_F_SOLID", "AXLE_F_MCPHERSON",
    "AXLE_F_REVERSE", "AXLE_R_NOTILT", "AXLE_R_SOLID", "AXLE_R_MCPHERSON", "AXLE_R_REVERSE", "IS_BIKE", "IS_HELI",
    "IS_PLANE", "IS_BOAT", "BOUNCE_PANELS",
    "DOUBLE_RWHEELS", "FORCE_GROUND_CLEARANCE", "IS_HATCHBAC1K"
};

VehCustmzrMgr& VehMod = VehCustmzrMgr::Get();

#ifdef GTASA
void VehCustmzrMgr::AddComponent(const std::string& component, const bool display_message)
{
    try
    {
        CPlayerPed* player = FindPlayerPed();
        int icomp = std::stoi(component);
        int hveh = CPools::GetVehicleRef(player->m_pVehicle);

        CStreaming::RequestModel(icomp, eStreamingFlags::PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(true);
        player->m_pVehicle->AddVehicleUpgrade(icomp);
        CStreaming::SetModelIsDeletable(icomp);

        if (display_message)
        {
            Util::SetMessage("Component added");
        }
    }
    catch (...)
    {
        Log::Print<eLogLevel::Warn>("Failed to add component to vehicle {}", component);
    }
}


void VehCustmzrMgr::RemoveComponent(const std::string& component, const bool display_message)
{
    try
    {
        CPlayerPed* player = FindPlayerPed();
        int icomp = std::stoi(component);
        int hveh = CPools::GetVehicleRef(player->m_pVehicle);

        player->m_pVehicle->RemoveVehicleUpgrade(icomp);

        if (display_message)
        {
            Util::SetMessage("Component removed");
        }
    }
    catch (...)
    {
        Log::Print<eLogLevel::Warn>("Failed to remove component from vehicle {}", component);
    }
}

bool VehCustmzrMgr::IsSideskirtComponent(unsigned int compID)
{
    static int models[] =
    {
        1007, 1026, 1027, 1030, 1031, 1036, 1039, 1040, 1041, 1042, 1047, 1048, 1051, 1052, 1056,
        1057, 1062, 1063, 1069, 1070, 1071, 1072, 1090, 1093, 1094, 1095, 1099, 1101, 1102, 1106, 1107, 1108, 1118, 1119,
        1120, 1121, 1122, 1124, 1133, 1134, 1137
    };
    static int maxSize = sizeof(models)/sizeof(models[0]);

    for (int j = 0; j != maxSize; ++j)
    {
        if (compID == models[j])
        {
            return true;
        }
    }

    return false;
}

void VehCustmzrMgr::ApplyCustomizations(std::string& cat, std::string& key, std::string& val)
{
    CVehicle *pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());
    if (pVeh)
    {
        int model = pVeh->m_nModelIndex;
        std::string name = Util::GetCarName(model);
        std::string index = std::format("{}.{}.", name, key);
        int reqModel = m_CustomizeData.m_pData->Get((index + "Model").c_str(), -1);

        if (model != reqModel)
        {
            Util::SetMessage(TEXT("Vehicle.Unsupported"));
            return;
        }

        // colors
        toml::array *temp = m_CustomizeData.m_pData->GetArray((index + "ColorMat").c_str());
        CRGBA col {static_cast<uchar>(temp->at(0).value_or(0)), static_cast<uchar>(temp->at(1).value_or(0)),
                   static_cast<uchar>(temp->at(2).value_or(0)), static_cast<uchar>(temp->at(3).value_or(0))};
        if (col.a != 0)
        {
            Paint.SetColor(pVeh, col);
        }

        temp = m_CustomizeData.m_pData->GetArray((index + "ColorCarcols").c_str());
        Paint.SetCarcols(pVeh, temp->at(0).value_or(0), temp->at(1).value_or(0), temp->at(2).value_or(0), temp->at(3).value_or(0), false);

        std::string texture = m_CustomizeData.m_pData->Get((index + "Texture").c_str(), "");
        if (texture != "")
        {
            Paint.SetTexture(pVeh, texture);
        }

        // neons
        temp = m_CustomizeData.m_pData->GetArray((index + "NeonColor").c_str());
        Neon.Install(pVeh, temp->at(0).value_or(0), temp->at(1).value_or(0), temp->at(2).value_or(0));
        bool pulsing = m_CustomizeData.m_pData->Get((index + "NeonPulsing").c_str(), false);
        Neon.SetPulsing(pVeh, pulsing);

        // tunes
        temp  = m_CustomizeData.m_pData->GetArray((index + "Tunes").c_str());
        for (size_t i = 0; i < temp->size(); ++i)
        {
            int compId = temp->at(i).value_or(-1);
            if (compId != -1)
            {
                AddComponent(std::to_string(compId), false);
            }
        }

        // paintjob
        int count;
        int hVeh = CPools::GetVehicleRef(pVeh);
        Command<Commands::GET_NUM_AVAILABLE_PAINTJOBS>(hVeh, &count);
        if (count > 0)
        {
            int val = m_CustomizeData.m_pData->Get((index + "PaintJob").c_str(), -1);
            Command<Commands::GIVE_VEHICLE_PAINTJOB>(hVeh, val);
        }
    }
}

void VehCustmzrMgr::SaveCustomizations()
{
    CVehicle *pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());
    if (pVeh)
    {
        int model = pVeh->m_nModelIndex;
        std::string name = Util::GetCarName(model);
        std::string index = std::format("{}.{} - {}.", name, name, m_nLabel);
        m_CustomizeData.m_pData->Set((index + "Model").c_str(), model);

        // Save colors
        auto paintData = Paint.Get().GetData(pVeh);
        RwRGBA color = {0, 0, 0, 0};

        // get a proper mat color
        for (auto item : paintData.m_nMapInfoList)
        {
            if (item.second.m_nColor.alpha != 0)
            {
                color = item.second.m_nColor;
                break;
            }
        }
        toml::array ColArr {color.red, color.green, color.blue, color.alpha};
        m_CustomizeData.m_pData->Set((index + "ColorMat").c_str(), ColArr);

        toml::array CarcolsArr {paintData.m_nCarColors[0], paintData.m_nCarColors[1],
                                paintData.m_nCarColors[2], paintData.m_nCarColors[3]};
        m_CustomizeData.m_pData->Set((index + "ColorCarcols").c_str(), CarcolsArr);
        m_CustomizeData.m_pData->Set((index + "Texture").c_str(), paintData.m_nTextureName);

        // Save neons
        auto neonData = Neon.Get().GetData(pVeh);
        toml::array neonArr {neonData.m_Color.r, neonData.m_Color.g, neonData.m_Color.b};
        m_CustomizeData.m_pData->Set((index + "NeonColor").c_str(), neonArr);
        m_CustomizeData.m_pData->Set((index + "NeonPulsing").c_str(), neonData.m_bPulsing);


        // Save tunes
        toml::array tune;
        for (size_t i = 0; i < IM_ARRAYSIZE(pVeh->m_anUpgrades); ++i)
        {
            int compId = pVeh->m_anUpgrades[i];
            if (compId != -1)
            {
                tune.push_back(compId);
            }
        }
        m_CustomizeData.m_pData->Set((index + "Tunes").c_str(), tune);

        // Save paintjob
        int count, curpJob;
        int hVeh = CPools::GetVehicleRef(pVeh);
        Command<Commands::GET_NUM_AVAILABLE_PAINTJOBS>(hVeh, &count);
        if (count > 0)
        {
            Command<Commands::GET_CURRENT_VEHICLE_PAINTJOB>(hVeh, &curpJob);
            m_CustomizeData.m_pData->Set((index + "PaintJob").c_str(), curpJob);
        }
        m_CustomizeData.m_pData->Save();
        m_CustomizeData.UpdateSearchList();
    }
}
#endif

VehCustmzrMgr& VehCustmzr = VehCustmzrMgr::Get();

VehCustmzrMgr::VehCustmzrMgr()
{
    FileHandler::FetchColorData(m_ColorData);

#ifdef GTASA
    m_CustomizeData.m_bAllowRemoveAll = true;
    FileHandler::FetchHandlingID(m_VehicleIDE);

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
                int flag = 0;
                unsigned int start = 0, end = 0;

                if (veh->m_nVehicleClass == CLASS_NORMAL)
                {
                    start = 1;
                    end = 20;
                }

                if (veh->m_nVehicleClass == CLASS_RICHFAMILY) 
                {
                    start = 1;
                    end = 4;
                }

                if (veh->m_nVehicleClass == CLASS_EXECUTIVE) 
                {
                    start = 1;
                    end = 3;
                }

                if (start != end)
                {
                    flag = Random(start, end);

                    if (flag == 1 && !Neon.IsInstalled(veh) && veh->m_pDriver != pPlayer)
                    {
                        int r = static_cast<int>(Random(0U, 255U));
                        int g = static_cast<int>(Random(0U, 255U));
                        int b = static_cast<int>(Random(0U, 255U));
                        Neon.Install(veh, r, g, b);
                    }
                }
            }
            m_Neon.m_nTrafficTimer = timer;
        }

        if (m_Nitro.m_bEnabled && pVeh && pVeh->m_nVehicleSubClass == VEHICLE_AUTOMOBILE)
        {
            patch::Set<BYTE>(0x969165, 0, true); // All cars have nitro
            patch::Set<BYTE>(0x96918B, 0, true); // All taxis have nitro

            if (KeyPressed(VK_LBUTTON))
            {
                if (!m_Nitro.m_bCompAdded)
                {
                    AddComponent("1010", false);
                    m_Nitro.m_bCompAdded = true;
                }
            }
            else
            {
                if (m_Nitro.m_bCompAdded)
                {
                    RemoveComponent("1010", false);
                    m_Nitro.m_bCompAdded = false;
                }
            }
        }
    };
#endif
}

#ifdef GTASA
bool VehCustmzrMgr::IsValidComponent(CVehicle *pVeh, unsigned int compID)
{
    return CallAndReturn<bool, 0x49B010, int, CVehicle*>(compID, pVeh);
}
#endif

void VehCustmzrMgr::Draw()
{
    CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());
    int hVeh = CPools::GetVehicleRef(pVeh);

    if (ImGui::BeginTabItem(TEXT( "Vehicle.Color")))
    {
#ifdef GTASA
        ImGui::Spacing();
        if (ImGui::Button(TEXT("Vehicle.ResetColor"), ImVec2(Widget::CalcSize())))
        {
            Paint.ResetColor(pVeh);
            Util::SetMessage(TEXT("Vehicle.ResetColorMSG"));
        }
        ImGui::Spacing();

        static float colpicker[3];
        if (ImGui::ColorEdit3(TEXT("Vehicle.ColorPicker"), colpicker))
        {
            uchar r = colpicker[0] * 255;
            uchar g = colpicker[1] * 255;
            uchar b = colpicker[2] * 255;
            Paint.SetColor(pVeh, { r, g, b, 255 });
        }
#endif

        ImGui::Spacing();

        static int colorType = 0;
#ifdef GTASA
        ImGui::Combo(TEXT("Vehicle.ColorType"), &colorType, "Primary\0Secondary\0Tertiary\0Quaternary\0");
#else
        ImGui::Combo(TEXT("Vehicle.ColorType"), &colorType, "Primary\0Secondary\0");
#endif
        ImGui::Spacing();
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
                *(uint8_replacement*)(int(pVeh) + BY_GAME(0x433, 0x19F, 0x19B) + colorType + 1) = colorId;
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
            if (Widget::Toggle(TEXT("Vehicle.PulsingNeon"), &pulsing))
            {
                Neon.SetPulsing(pVeh, pulsing);
            }

            Widget::Toggle(TEXT("Vehicle.RainbowNeon"), &m_Neon.m_bRainbowEffect, TEXT("Vehicle.RainbowNeonMSG"));
            ImGui::NextColumn();
            Widget::Toggle(TEXT("Vehicle.TrafficNeon"), &m_Neon.m_bApplyOnTraffic, TEXT("Vehicle.TrafficNeonMSG"));
            ImGui::Columns(1);

            ImGui::Spacing();
            static float colorPicker[3] {0, 0, 0};
            if (ImGui::ColorEdit3(TEXT("Vehicle.ColorPicker"), colorPicker))
            {
                int r = static_cast<int>(colorPicker[0] * 255);
                int g = static_cast<int>(colorPicker[1] * 255);
                int b = static_cast<int>(colorPicker[2] * 255);

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

        if (ImGui::BeginTabItem(TEXT( "Vehicle.TextureTab")))
        {
            ImGui::Spacing();
            if (ImGui::Button(TEXT("Vehicle.ResetTexture"), ImVec2(Widget::CalcSize())))
            {
                Paint.ResetTexture(pVeh);
                Util::SetMessage(TEXT("Vehicle.ResetTextureMSG"));
            }
            ImGui::Spacing();

            ImGui::Columns(2, NULL, false);
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
            Widget::ImageList(Paint.m_TextureData,
                              [this](std::string& str)
            {
                Paint.SetTexture(FindPlayerPed()->m_pVehicle, str);
            },
            [](std::string& str)
            {
                return str;
            }, nullptr, []() {});

            ImGui::EndTabItem();
        }
    }
    if (ImGui::BeginTabItem(TEXT( "Vehicle.TuneTab")))
    {
        ImGui::Spacing();
        if (ImGui::Button(TEXT("Vehicle.RemoveTune"), ImVec2(Widget::CalcSize())))
        {
            for (int i = 0; i != 15; ++i)
            {
                int compID = pVeh->GetUpgrade(i);
                if (compID > -1)
                {
                    pVeh->RemoveVehicleUpgrade(compID);
                }
            }

            /*
                Remove leftover comps
                Nitro, wheels, hydralics
            */
            int comps[] = {1008, 1009, 1010, 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087};
            int maxSize = sizeof(comps)/sizeof(comps[0]);
            for (int i = 0; i < maxSize; ++i)
            {
                pVeh->RemoveVehicleUpgrade(comps[i]);
            }

            Util::SetMessage(TEXT("Vehicle.RemoveTuneMSG"));
        }
        ImGui::Spacing();

        static CVehicle *prevVeh = nullptr;
        CVehicle *curVeh = FindPlayerPed()->m_pVehicle;
        if (prevVeh != curVeh)
        {
            m_TuneData.m_bSearchListUpdateRequired = true;
            prevVeh = curVeh;
        }

        Widget::ImageList(m_TuneData,
                          [this](std::string& str)
        {
            AddComponent(str);
        },
        // [](std::string& str)
        // {
        //     RemoveComponent(str);
        // },
        [](std::string& str)
        {
            return str;
        },
        [](std::string& str)
        {
            return VehCustmzr.IsValidComponent(FindPlayerPed()->m_pVehicle, std::stoi(str));
        });

        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(TEXT( "Vehicle.HandlingTab")))
    {
        ImGui::Spacing();
        // https://github.com/multitheftauto/mtasa-blue/blob/16769b8d1c94e2b9fe6323dcba46d1305f87a190/Client/game_sa/CModelInfoSA.h#L213
        CBaseModelInfo* pInfo = CModelInfo::GetModelInfo(FindPlayerVehicle(-1, false)->m_nModelIndex);
        int handlingID = patch::Get<WORD>((int)pInfo + 74, false); //  CBaseModelInfo + 74 = handlingID
        tHandlingData *pHandlingData = reinterpret_cast<tHandlingData*>(0xC2B9DC + (handlingID * 224)); // sizeof(tHandlingData) = 224

        if (ImGui::Button(TEXT("Vehicle.ResetHandling"), ImVec2(Widget::CalcSize(3))))
        {
            gHandlingDataMgr.LoadHandlingData();
            Util::SetMessage(TEXT("Vehicle.ResetHandlingMSG"));
        }

        ImGui::SameLine();

        if (ImGui::Button(TEXT("Vehicle.SaveFile"), ImVec2(Widget::CalcSize(3))))
        {
            FileHandler::GenerateHandlingFile(pHandlingData, m_VehicleIDE);
            Util::SetMessage(TEXT("Vehicle.SaveFileMSG"));
        }

        ImGui::SameLine();

        if (ImGui::Button(TEXT("Vehicle.ReadMore"), ImVec2(Widget::CalcSize(3))))
        {
            ShellExecute(NULL, "open", "https://projectcerbera.com/gta/sa/tutorials/handling", NULL, NULL,
                         SW_SHOWNORMAL);
        }

        ImGui::Spacing();

        ImGui::BeginChild("HandlingChild");

        std::vector<Widget::BindInfo> abs{ {TEXT("Vehicle.On"), 1}, {TEXT("Vehicle.Off"), 0} };
        Widget::EditRadioBtnAddr(TEXT("Vehicle.Abs"), (int)&pHandlingData->m_bABS, abs);

        Widget::EditAddr(TEXT("Vehicle.ADM"), (int)&pHandlingData->m_fSuspensionAntiDiveMultiplier, 0.0f, 0.0f, 1.0f);
        Widget::EditAddr<BYTE>(TEXT("Vehicle.AnimGroup"), (int)&pHandlingData->m_nAnimGroup, 0, 0, 20);
        Widget::EditAddr(TEXT("Vehicle.BrakeBias"), (int)&pHandlingData->m_fBrakeBias, 0.0f, 0.0f, 1.0f);

        // Brake deceleration calculation
        float BrakeDeceleration = pHandlingData->m_fBrakeDeceleration * 2500;
        Widget::EditAddr(TEXT("Vehicle.BrakeDecel"), (int)&pHandlingData->m_fBrakeDeceleration, 0.0f, 0.0f, 20.0f, 2500.0f);
        pHandlingData->m_fBrakeDeceleration = BrakeDeceleration / 2500;

        Widget::EditAddr(TEXT("Vehicle.CemterMassX"), (int)&pHandlingData->m_vecCentreOfMass.x, -10.0f, -10.0f, 10.0f);
        Widget::EditAddr(TEXT("Vehicle.CemterMassY"), (int)&pHandlingData->m_vecCentreOfMass.y, -10.0f, -10.0f, 10.0f);
        Widget::EditAddr(TEXT("Vehicle.CemterMassZ"), (int)&pHandlingData->m_vecCentreOfMass.z, -10.0f, -10.0f, 10.0f);

        // CDM calculations
        float factor = (1.0 / pHandlingData->m_fMass);
        float fCDM = pHandlingData->m_fCollisionDamageMultiplier / (2000.0f * factor);
        Widget::EditAddr(TEXT("Vehicle.CDM"), (int)&fCDM, 0.0f, 0.0f, 1.0f, 0.3381f);
        pHandlingData->m_fCollisionDamageMultiplier = factor * fCDM * 2000.0f;

        Widget::EditAddr(TEXT("Vehicle.DampingLvl"), (int)&pHandlingData->m_fSuspensionDampingLevel, -10.0f, -10.0f, 10.0f); // test later
        Widget::EditAddr(TEXT("Vehicle.DragMult"), (int)&pHandlingData->m_fDragMult, 0.0f, 0.0f, 30.0f);

        std::vector<Widget::BindInfo> drive_type
        {
            {TEXT("Vehicle.FrontWheelDrive"), 70},
            {TEXT("Vehicle.RearWheelDrive"), 82},
            {TEXT("Vehicle.FourWheelDrive"), 52}
        };
        Widget::EditRadioBtnAddr(TEXT("Vehicle.DriveType"), (int)&pHandlingData->m_transmissionData.m_nDriveType, drive_type);

        // Engine acceleration calculation
        float fEngineAcceleration = pHandlingData->m_transmissionData.m_fEngineAcceleration * 25000.0f;
        fEngineAcceleration /= pHandlingData->m_transmissionData.m_nDriveType == 52 ? 4.0f : 2.0f;
        Widget::EditAddr(TEXT("Vehicle.EngineAccel"), (int)&fEngineAcceleration, 0.0f, 0.0f, 49.0f, 1.0f);
        fEngineAcceleration *= pHandlingData->m_transmissionData.m_nDriveType == 52 ? 4.0f : 2.0f;
        pHandlingData->m_transmissionData.m_fEngineAcceleration = fEngineAcceleration / 25000.0f;


        Widget::EditAddr(TEXT("Vehicle.EngineInertia"), (int)&pHandlingData->m_transmissionData.m_fEngineInertia, 0.1f, 0.1f, 400.0f);

        std::vector<Widget::BindInfo> engine_type
        {
            {TEXT("Vehicle.Petrol"), 80}, {TEXT("Vehicle.Diesel"), 68}, {TEXT("Vehicle.Electric"), 69}
        };
        Widget::EditRadioBtnAddr(TEXT("Vehicle.EngineType"), (int)&pHandlingData->m_transmissionData.m_nEngineType, engine_type);

        std::vector<Widget::BindInfo> lights
        {
            {TEXT("Vehicle.Long"), 0}, {TEXT("Vehicle.Small"), 1},
            {TEXT("Vehicle.Big"), 2}, {TEXT("Vehicle.Tall"), 3}
        };
        Widget::EditRadioBtnAddr(TEXT("Vehicle.FrontLights"), (int)&pHandlingData->m_nFrontLights, lights);

        Widget::EditAddr(TEXT("Vehicle.ForceLevel"), (int)&pHandlingData->m_fSuspensionForceLevel, -10.0f, -10.0f, 10.0f); // test later

        Widget::EditBits(TEXT("Vehicle.HandlingFlags"), (int)&pHandlingData->m_nHandlingFlags, m_HandlingFlagNames);

        Widget::EditAddr(TEXT("Vehicle.HighSpeedDamping"), (int)&pHandlingData->m_fSuspensionDampingLevel, -10.0f, -10.0f, 10.0f); // test later
        Widget::EditAddr(TEXT("Vehicle.LowerLimit"), (int)&pHandlingData->m_fSuspensionLowerLimit, -10.0f, -10.0f, 10.0f); // test later
        Widget::EditAddr(TEXT("Vehicle.Mass"), (int)&pHandlingData->m_fMass, 1.0f, 1.0f, 50000.0f);

        // Max Velocity calculation
        int MaxVelocity = pHandlingData->m_transmissionData.m_fMaxGearVelocity / *(float*)0xC2B9BC;
        Widget::EditAddr(TEXT("Vehicle.MaxVelocity"), (int)&MaxVelocity, 1.0f, 1.0f, 1000.0f);
        pHandlingData->m_transmissionData.m_fMaxGearVelocity = MaxVelocity * (*(float*)0xC2B9BC);

        Widget::EditBits(TEXT("Vehicle.ModelFlags"), (int)&pHandlingData->m_nModelFlags, m_ModelFlagNames);

        Widget::EditAddr<int>(TEXT("Vehicle.MonValue"), (int)&pHandlingData->m_nMonetaryValue, 1, 1, 100000);
        Widget::EditAddr<BYTE>(TEXT("Vehicle.NumGears"), (int)&pHandlingData->m_transmissionData.m_nNumberOfGears, 1, 1, 10);
        Widget::EditAddr<BYTE>(TEXT("Vehicle.PercentSubmerged"), (int)&pHandlingData->m_nPercentSubmerged, 10, 10, 120);

        Widget::EditRadioBtnAddr(TEXT("Vehicle.RearLights"), (int)&pHandlingData->m_nRearLights, lights);

        Widget::EditAddr(TEXT("Vehicle.SeatOffset"), (int)&pHandlingData->m_fSeatOffsetDistance, 0.0f, 0.0f, 1.0f);
        Widget::EditAddr(TEXT("Vehicle.SteeringLock"), (int)&pHandlingData->m_fSteeringLock, 10.0f, 10.0f, 50.0f);
        Widget::EditAddr(TEXT("Vehicle.SuspensionBias"), (int)&pHandlingData->m_fSuspensionBiasBetweenFrontAndRear, 0.0f, 0.0f, 1.0f);
        Widget::EditAddr(TEXT("Vehicle.TractionBias"), (int)&pHandlingData->m_fTractionBias, 0.0f, 0.0f, 1.0f);
        Widget::EditAddr(TEXT("Vehicle.TractionLoss"), (int)&pHandlingData->m_fTractionLoss, 0.0f, 0.0f, 1.0f);
        Widget::EditAddr(TEXT("Vehicle.TractionMul"), (int)&pHandlingData->m_fTractionMultiplier, 0.5f, 0.5f, 2.0f);
        Widget::EditAddr(TEXT("Vehicle.TurnMass"), (int)&pHandlingData->m_fTurnMass, 20.0f, 20.0f, 1000.0f); // test later
        Widget::EditAddr(TEXT("Vehicle.UpperLimit"), (int)&pHandlingData->m_fSuspensionUpperLimit, -1.0f, -1.0f, 1.0f);

        ImGui::EndChild();

        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(TEXT( "Vehicle.Save")))
    {
        ImGui::Spacing();
        ImGui::InputText(TEXT("Menu.Name"), m_nLabel, IM_ARRAYSIZE(m_nLabel));
        ImGui::Spacing();
        if (ImGui::Button(TEXT("Vehicle.CutomizeSave"), Widget::CalcSize()))
        {
            SaveCustomizations();
            Util::SetMessage(TEXT("Vehicle.CutomizeSaveMSG"));
        }
        ImGui::Spacing();
        ImGui::Spacing();

        Widget::DataList(m_CustomizeData, fArg3Wrapper(VehCustmzr.ApplyCustomizations));
        ImGui::EndTabItem();
    }
#endif
}