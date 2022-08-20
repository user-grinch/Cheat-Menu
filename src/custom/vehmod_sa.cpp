#include "pch.h"
#include "vehmod_sa.h"
#include "utils/widget.h"
#include "filehandler.h"

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

VehModMgr& VehMod = VehModMgr::Get();

VehModMgr::VehModMgr()
{
    FileHandler::FetchHandlingID(m_VehicleIDE);

    Events::processScriptsEvent += [this]()
    {
        if (m_Nitro.m_bEnabled && FindPlayerVehicle(-1, false)->m_nVehicleSubClass == VEHICLE_AUTOMOBILE)
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
}

void VehModMgr::AddComponent(const std::string& component, const bool display_message)
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


void VehModMgr::RemoveComponent(const std::string& component, const bool display_message)
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

void VehModMgr::Draw()
{
    if (ImGui::BeginTabItem(TEXT("Vehicle.TuneTab")))
    {
        ImGui::Spacing();
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
            return ((bool(*)(int, CVehicle*))0x49B010)(std::stoi(str), FindPlayerPed()->m_pVehicle);
        });

        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(TEXT("Vehicle.HandlingTab")))
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
        float fEngineAcceleration = pHandlingData->m_transmissionData.m_fEngineAcceleration * 12500;
        Widget::EditAddr(TEXT("Vehicle.EngineAccel"), (int)&fEngineAcceleration, 0.0f, 0.0f, 49.0f, 12500.0f);
        pHandlingData->m_transmissionData.m_fEngineAcceleration = fEngineAcceleration / 12500;


        Widget::EditAddr(TEXT("Vehicle.EngineInertia"), (int)&pHandlingData->m_transmissionData.m_fEngineInertia, 0.1f, 0.1f, 400.0f);

        std::vector<Widget::BindInfo> engine_type
        { 
            {TEXT("Vehicle.Petrol"), 80}, {TEXT("Vehicle.Diseal"), 68}, {TEXT("Vehicle.Electric"), 69} 
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
        Widget::EditAddr(TEXT("Vehicle.LowerKimit"), (int)&pHandlingData->m_fSuspensionLowerLimit, -10.0f, -10.0f, 10.0f); // test later
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
}