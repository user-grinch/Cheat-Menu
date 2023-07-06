#include "pch.h"
#include "vehicle.h"
#include <CPopulation.h>
#include <CDamageManager.h>
#include "menu.h"
#include "teleport.h"
#include "utils/widget.h"
#include "utils/util.h"
#include "custom/filehandler.h"
#include "custom/vehcustmzr.h"
#include "custom/autodrive.h"
#include <CMatrix.h>

VehiclePage& vehiclePage = VehiclePage::Get();
VehiclePage::VehiclePage()
    : IPage<VehiclePage>(ePageID::Vehicle, "Vehicle", true)
{
    // Get config data
    Events::initGameEvent += [this]()
    {
        m_Spawner.m_bInAir = gConfig.Get("Features.SpawnAircraftInAir", true);
        m_Spawner.m_bAsDriver = gConfig.Get("Features.SpawnInsideVehicle", true);
        m_Spawner.m_bWithTunes = gConfig.Get("Features.SpawnWithTunes", true);
    };

    Events::processScriptsEvent += [this]
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        CPlayerPed* pPlayer = FindPlayerPed();
        CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());

        if (pPlayer && Util::IsInCar())
        {
            int hveh = CPools::GetVehicleRef(pVeh);
            float speed = pVeh->m_vecMoveSpeed.Magnitude() * 50.0f;
            if (m_bAutoUnflip && pVeh->IsUpsideDown() && speed < 2.0f)
            {
                Util::UnFlipCar(pVeh);
            }

            if (unflipVeh.Pressed())
            {
                Util::UnFlipCar(pVeh);
            }

            if (fixVeh.Pressed())
            {
                Util::FixCar(pVeh);
                Util::SetMessage("Vehicle fixed");
            }

            if (vehEngine.Pressed())
            {
                bool state = BY_GAME(!pVeh->m_nVehicleFlags.bEngineBroken, true, true) || pVeh->m_nVehicleFlags.bEngineOn;

                if (state)
                {
                    Util::SetMessage("Vehicle engine off");
                }
                else
                {
                    Util::SetMessage("Vehicle engine on");
                }
#ifdef GTASA
                pVeh->m_nVehicleFlags.bEngineBroken = state;
#endif
                pVeh->m_nVehicleFlags.bEngineOn = !state;
            }

            if (vehInstantStart.Pressed())
            {
                Util::SetCarForwardSpeed(pVeh, 40.0f);
            }

            if (vehInstantStop.Pressed())
            {
                Util::SetCarForwardSpeed(pVeh, 0.0f);
            }

            if (m_bNoDamage)
            {
#ifdef GTASA
                pVeh->m_nPhysicalFlags.bBulletProof = true;
                pVeh->m_nPhysicalFlags.bExplosionProof = true;
                pVeh->m_nPhysicalFlags.bFireProof = true;
                pVeh->m_nPhysicalFlags.bCollisionProof = true;
                pVeh->m_nPhysicalFlags.bMeleeProof = true;
                pVeh->m_nVehicleFlags.bTyresDontBurst = true;
                pVeh->m_nVehicleFlags.bCanBeDamaged = true;
#elif GTAVC
                pVeh->m_nFlags.bBulletProof = true;
                pVeh->m_nFlags.bExplosionProof = true;
                pVeh->m_nFlags.bFireProof = true;
                pVeh->m_nFlags.bCollisionProof = true;
                pVeh->m_nFlags.bMeleeProof = true;
                pVeh->m_nFlags.bImmuneToNonPlayerDamage = true;

                // patch for bikes
                patch::Nop(0x614E20, 6);

                // no tyre burst
                patch::SetRaw(0x609F30, (void*)"\xC2\x08\x00", 3);
                patch::SetRaw(0x5886A0, (void*)"\xC2\x08\x00", 3);
#else
                pVeh->m_nFlags.bBulletProof = true;
                pVeh->m_nFlags.bExplosionProof = true;
                pVeh->m_nFlags.bFireProof = true;
                pVeh->m_nFlags.bCollisionProof = true;
                pVeh->m_nFlags.bMeleeProof = true;

                // no tyre burst
                patch::SetRaw(0x53C0E0, (void*)"\xC2\x04\x00", 3);
#endif
            }

            Command<Commands::SET_CAR_HEAVY>(hveh, m_bVehHeavy);
            Command<Commands::SET_CAR_WATERTIGHT>(hveh, m_bVehWatertight);

            if (m_bLockSpeed)
            {
                Util::SetCarForwardSpeed(pVeh, m_fLockSpeed);
            }
        }

#ifdef GTASA

        if (m_bBikeFly && pVeh && pVeh->IsDriver(pPlayer))
        {
            if (pVeh->m_nVehicleSubClass == VEHICLE_BIKE || pVeh->m_nVehicleSubClass == VEHICLE_BMX)
            {
                if (sqrt(pVeh->m_vecMoveSpeed.x * pVeh->m_vecMoveSpeed.x
                         + pVeh->m_vecMoveSpeed.y * pVeh->m_vecMoveSpeed.y
                         + pVeh->m_vecMoveSpeed.z * pVeh->m_vecMoveSpeed.z
                        ) > 0.0
                        && CTimer::ms_fTimeStep > 0.0)
                {
                    pVeh->FlyingControl(3, -9999.9902f, -9999.9902f, -9999.9902f, -9999.9902f);
                }
            }
        }
#endif
    };
}

#ifdef GTASA
// hardcoded for now
int VehiclePage::GetRandomTrainIdForModel(int model)
{
    static int train_ids[] =
    {
        8, 9, // model 449
        0, 3, 6, 10, 12, 13, // model 537
        1, 5, 15 // model 538
    };
    unsigned int start = 0, end = 0;

    switch (model)
    {
    case 449:
        start = 0;
        end = 1;
        break;
    case 537:
        start = 2;
        end = 7;
        break;
    case 538:
        start = 8;
        end = 10;
        break;
    default:
        Util::SetMessage("Invalid train model");
        return -1;
    }
    unsigned int id = Random(start, end);
    return train_ids[id];
}
#elif GTAVC
void WarpPlayerIntoVehicle(CVehicle *pVeh, int seatId)
{
    CPlayerPed *pPlayer = FindPlayerPed();
    pPlayer->m_bInVehicle = true;
    pPlayer->m_pVehicle = pVeh;
    pPlayer->RegisterReference((CEntity**)&pPlayer->m_pVehicle);
    pPlayer->m_pObjectiveVehicle = pVeh;
    pPlayer->RegisterReference((CEntity**)&pPlayer->m_pObjectiveVehicle);
    pPlayer->m_ePedState = ePedState::PEDSTATE_DRIVING;
    pPlayer->m_nObjective = OBJECTIVE_NO_OBJ;
    patch::Set<BYTE>(0x7838CD, 1); // player got in car flag
    Call<0x41D370>(pVeh); // CCarCtrl::RegisterVehicleOfInterest

    if (pVeh->m_passengers[seatId])
    {
        pVeh->m_passengers[seatId]->Remove();
    }
    pVeh->m_passengers[seatId] = pPlayer;
    pVeh->RegisterReference((CEntity**)&pVeh->m_passengers[seatId]);

    // Set player position
    CWorld::Remove(pPlayer);
    pPlayer->m_placement.pos.x = pVeh->m_placement.pos.x;
    pPlayer->m_placement.pos.y = pVeh->m_placement.pos.y;
    pPlayer->m_placement.pos.z = pVeh->m_placement.pos.z;
    CWorld::Add(pPlayer);

    pPlayer->m_nFlags.bUseCollision = false;
    pPlayer->m_nPedFlags.bIsStanding = false;
    pPlayer->m_nPedFlags.b03 = 0;
    RpAnimBlendClumpSetBlendDeltas(pPlayer->m_pRwClump, 16, -1000);
    pPlayer->AddInCarAnims(pVeh, false);
    CallMethod<0x4FF6A0>(pPlayer); // char __thiscall RemoveWeaponWhenEnteringVehicle(CPed*)
}
#endif

void VehiclePage::AddNew()
{
    static char name[INPUT_BUFFER_SIZE];
    static int model = 0;
    ImGui::InputTextWithHint(TEXT("Menu.Name"), "Cheetah", name, INPUT_BUFFER_SIZE);
    Widget::InputInt(TEXT("Vehicle.Model"), &model, 0, 999999);
    ImGui::Spacing();
    ImVec2 sz = Widget::CalcSize(2);
    if (ImGui::Button(TEXT("Window.AddEntry"), sz))
    {
        if (CModelInfo::IsCarModel(model))
        {
            std::string key = std::format("Custom.{} (Added)", name);
            m_Spawner.m_VehData.m_pData->Set(key.c_str(), std::to_string(model));
            m_Spawner.m_VehData.m_pData->Save();
            Util::SetMessage(TEXT("Window.AddEntryMSG"));
        }
        else
        {
            Util::SetMessage(TEXT("Vehicle.InvalidID"));
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(TEXT("Vehicle.GetCurrentVehInfo"), sz))
    {
        CPlayerPed *pPlayer = FindPlayerPed();
        int hPlayer = CPools::GetPedRef(pPlayer);
        if (Command<Commands::IS_CHAR_IN_ANY_CAR>(hPlayer))
        {
            model = pPlayer->m_pVehicle->m_nModelIndex;
            std::string str = Util::GetCarName(model);
            strcpy(name, str.c_str());
        }
        else
        {
            Util::SetMessage(TEXT("Vehicle.NotInVehicle"));
        }
    }
}

#ifdef GTASA
void VehiclePage::SpawnVehicle(std::string& smodel)
{
    CPlayerPed* player = FindPlayerPed();
    int hplayer = CPools::GetPedRef(player);
    int imodel = std::stoi(smodel);
    CVehicle* pVeh = nullptr;

    CVector pos = player->GetPosition();
    float speed = 0;

    bool bInVehicle = Command<Commands::IS_CHAR_IN_ANY_CAR>(hplayer);
    if (bInVehicle && m_Spawner.m_bAsDriver)
    {
        CVehicle* pVeh = player->m_pVehicle;
        int hVeh = CPools::GetVehicleRef(pVeh);
        pos = pVeh->GetPosition();

        Command<Commands::GET_CAR_SPEED>(hVeh, &speed);
        Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(hplayer, pos.x, pos.y, pos.z);

        if (pVeh->m_nVehicleClass == VEHICLE_TRAIN)
        {
            Command<Commands::DELETE_MISSION_TRAIN>(hVeh);
        }
        else
        {
            Command<Commands::DELETE_CAR>(hVeh);
        }
    }

    if (player->m_nAreaCode == 0)
    {
        if (m_Spawner.m_bInAir && (CModelInfo::IsHeliModel(imodel) || CModelInfo::IsPlaneModel(imodel)))
        {
            pos.z = 400;
        }
        else
        {
            pos.z -= 5;
        }
    }

    if (CModelInfo::IsTrainModel(imodel))
    {
        int trainID = GetRandomTrainIdForModel(imodel);

        if (trainID == -1) // Unknown train id
        {
            return;
        }

        int hVeh = 0;

        // Loading all train related models
        CStreaming::RequestModel(590, PRIORITY_REQUEST);
        CStreaming::RequestModel(538, PRIORITY_REQUEST);
        CStreaming::RequestModel(570, PRIORITY_REQUEST);
        CStreaming::RequestModel(569, PRIORITY_REQUEST);
        CStreaming::RequestModel(537, PRIORITY_REQUEST);
        CStreaming::RequestModel(449, PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(false);

        CTrain* pTrain = nullptr;
        CTrain* carraige = nullptr;
        int track = static_cast<int>(Random(0U, 1U));
        int node = CTrain::FindClosestTrackNode(pos, &track);
        CTrain::CreateMissionTrain(pos, (Random(0U, 1U)) == 1U ? true : false, trainID, &pTrain, &carraige, node, track, false);

        pVeh = (CVehicle*)pTrain;
        hVeh = CPools::GetVehicleRef(pVeh);
        if (pVeh->m_pDriver)
        {
            Command<Commands::DELETE_CHAR>(CPools::GetPedRef(pVeh->m_pDriver));
        }

        if (m_Spawner.m_bAsDriver)
        {
            Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hVeh);
            Util::SetCarForwardSpeed(pVeh, speed);
        }
        Command<Commands::MARK_MISSION_TRAIN_AS_NO_LONGER_NEEDED>(hVeh);
        Command<Commands::MARK_CAR_AS_NO_LONGER_NEEDED>(hVeh);
        CStreaming::SetModelIsDeletable(590);
        CStreaming::SetModelIsDeletable(538);
        CStreaming::SetModelIsDeletable(570);
        CStreaming::SetModelIsDeletable(569);
        CStreaming::SetModelIsDeletable(537);
        CStreaming::SetModelIsDeletable(449);
    }
    else
    {
        *CVehicleModelInfo::ms_compsToUse = m_nVehicleVariant;
        CStreaming::RequestModel(imodel, PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(false);

        if (m_Spawner.m_nLicenseText[0] != '\0')
        {
            Command<Commands::CUSTOM_PLATE_FOR_NEXT_CAR>(imodel, m_Spawner.m_nLicenseText);
        }

        int hveh = 0;
        if (m_Spawner.m_bAsDriver)
        {
            Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 4.0f, &hveh);
            pVeh = CPools::GetVehicle(hveh);
            pVeh->SetHeading(player->GetHeading());
            Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hveh);
            Util::SetCarForwardSpeed(pVeh, speed);
        }
        else
        {
            player->TransformFromObjectSpace(pos, CVector(0, 10, 0));
            Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 3.0f, &hveh);
            pVeh = CPools::GetVehicle(hveh);
            pVeh->SetHeading(player->GetHeading() + 55.0f);
        }

        // Add random tunes
        if (m_Spawner.m_bWithTunes && pVeh->m_nVehicleSubClass <= VEHICLE_QUAD)
        {
            for (int i = 0; i < 20; ++i)
            {
                unsigned int compID = Random(1000U, 1093U);

                if (VehCustmzr.IsSideskirtComponent(compID))
                {
                    continue;
                }

                if (VehCustmzr.IsValidComponent(pVeh, compID))
                {
                    CStreaming::RequestModel(compID, eStreamingFlags::PRIORITY_REQUEST);
                    CStreaming::LoadAllRequestedModels(true);
                    pVeh->AddVehicleUpgrade(compID);
                    CStreaming::SetModelIsDeletable(compID);
                }
            }
        }

        pVeh->m_eDoorLock = DOORLOCK_UNLOCKED;
        pVeh->m_nAreaCode = player->m_nAreaCode;
        Command<Commands::MARK_CAR_AS_NO_LONGER_NEEDED>(CPools::GetVehicleRef(pVeh));
        CStreaming::SetModelIsDeletable(imodel);
    }
    pVeh->m_nVehicleFlags.bHasBeenOwnedByPlayer = true;
}

#else

void VehiclePage::SpawnVehicle(std::string& rootkey, std::string& vehName, std::string& smodel)
{
    CPlayerPed* player = FindPlayerPed();
    int hplayer = CPools::GetPedRef(player);
    int imodel = std::stoi(smodel);
    CVehicle* veh = nullptr;
    int interior = BY_GAME(player->m_nAreaCode, player->m_nAreaCode, NULL);

    CVector pos = player->GetPosition();
    float speed = 0;

    bool bInVehicle = Command<Commands::IS_CHAR_IN_ANY_CAR>(hplayer);
    if (bInVehicle && m_Spawner.m_bAsDriver)
    {
        CVehicle* pveh = player->m_pVehicle;
        int hveh = CPools::GetVehicleRef(pveh);
        pos = pveh->GetPosition();

        Command<Commands::GET_CAR_SPEED>(hveh, &speed);
        Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(hplayer, pos.x, pos.y, pos.z);
        Command<Commands::DELETE_CAR>(hveh);
    }

    if (interior == 0)
    {
        if (m_Spawner.m_bInAir && (CModelInfo::IsHeliModel(imodel) || CModelInfo::IsPlaneModel(imodel)))
        {
            pos.z = 400;
        }
        else
        {
            pos.z -= 5;
        }
    }
#ifdef GTAVC
        *CVehicleModelInfo::ms_compsToUse = m_nVehicleVariant;
#endif

    CStreaming::RequestModel(imodel, PRIORITY_REQUEST);
    CStreaming::LoadAllRequestedModels(false);
    int hveh = 0;
    if (m_Spawner.m_bAsDriver)
    {
        Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 4.0f, &hveh);
        veh = CPools::GetVehicle(hveh);
#ifdef GTAVC
        float x,y,z;
        player->m_placement.GetOrientation(x, y, z);
        veh->m_placement.SetOrientation(x, y, z);
        Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hveh);
#else
        float x,y,z;
        player->GetOrientation(x, y, z);
        player->SetObjective(OBJECTIVE_ENTER_CAR_AS_DRIVER);
        player->WarpPedIntoCar(veh);
        veh->SetOrientation(x, y, z);
#endif

        Util::SetCarForwardSpeed(veh, speed);
    }
    else
    {
        player->TransformFromObjectSpace(pos);
        Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 4.0f, &hveh);
        veh = CPools::GetVehicle(hveh);
#ifdef GTAVC
        float x,y,z;
        player->m_placement.GetOrientation(x, y, z);
        veh->m_placement.SetOrientation(x, y, z);
#else
        float x,y,z;
        player->GetOrientation(x, y, z);
        veh->SetOrientation(x, y, z);
#endif
    }
    veh->m_eDoorLock = DOORLOCK_UNLOCKED;
#ifdef GTAVC
    BY_GAME(veh->m_nAreaCode, veh->m_nAreaCode, NULL) = interior;
#endif
    Command<Commands::MARK_CAR_AS_NO_LONGER_NEEDED>(CPools::GetVehicleRef(veh));
    CStreaming::SetModelIsDeletable(imodel);
    Command<Commands::RESTORE_CAMERA_JUMPCUT>();
}
#endif

void VehiclePage::Draw()
{
    ImGui::Spacing();
    CPlayerPed* pPlayer = FindPlayerPed();
    int hplayer = CPools::GetPedRef(pPlayer);
    bool bPlayerInCar = Command<Commands::IS_CHAR_IN_ANY_CAR>(hplayer);
    CVehicle *pVeh = pPlayer->m_pVehicle;

    if (ImGui::Button(TEXT("Vehicle.BlowCar"), ImVec2(Widget::CalcSize(3))))
    {
        for (CVehicle *pVeh : CPools::ms_pVehiclePool)
        {
            BY_GAME(pVeh->BlowUpCar(pPlayer, false), pVeh->BlowUpCar(pPlayer), pVeh->BlowUpCar(pPlayer));
        }
    }

    ImGui::SameLine();

    if (ImGui::Button(TEXT("Vehicle.FixCar"), ImVec2(Widget::CalcSize(3))) && Util::IsInCar())
    {
        Util::FixCar(pVeh);
    }

    ImGui::SameLine();

    if (ImGui::Button(TEXT("Vehicle.FlipCar"), ImVec2(Widget::CalcSize(3))) && Util::IsInCar())
    {
        Util::UnFlipCar(pVeh);
    }

    ImGui::Spacing();

    if (ImGui::BeginTabBar("Vehicle", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    {
        CVehicle* pVeh = pPlayer->m_pVehicle;
        bool is_driver = pVeh && (pPlayer->m_pVehicle->m_pDriver == pPlayer);

        ImGui::Spacing();

        if (ImGui::BeginTabItem(TEXT( "Window.ToggleTab")))
        {
            ImGui::BeginChild("CheckboxesChild");
            ImGui::Spacing();
            ImGui::Columns(2, 0, false);
#ifdef GTASA
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.AimDrive"), 0x969179);
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.AllNitro"), 0x969165);
#endif

            Widget::Toggle(TEXT("Vehicle.AutoUnflip"), &m_bAutoUnflip);

#ifndef GTA3
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.AggroDriver"), BY_GAME(0x96914F,0xA10B47, NULL));
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.AllTaxiNitro"), BY_GAME(0x96918B,0xA10B3A, NULL));
            Widget::Toggle(TEXT("Vehicle.BikeFly"), &m_bBikeFly);
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.BoatFly"), BY_GAME(0x969153, 0xA10B11, NULL));
#endif
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.CarFly"), BY_GAME(0x969160, 0xA10B28, 0x95CD75));
            Widget::Toggle(TEXT("Vehicle.CarHeavy"), &m_bVehHeavy);
            if (Widget::Toggle(TEXT("Vehicle.DmgProof"), &m_bNoDamage, TEXT("Vehicle.DmgProofTip")))
            {
                if (pVeh && !m_bNoDamage)
                {
#ifdef GTASA
                    pVeh->m_nPhysicalFlags.bBulletProof = false;
                    pVeh->m_nPhysicalFlags.bExplosionProof = false;
                    pVeh->m_nPhysicalFlags.bFireProof = false;
                    pVeh->m_nPhysicalFlags.bCollisionProof = false;
                    pVeh->m_nPhysicalFlags.bMeleeProof = false;
                    pVeh->m_nVehicleFlags.bTyresDontBurst = false;
                    pVeh->m_nVehicleFlags.bCanBeDamaged = false;
#elif GTAVC
                    pVeh->m_nFlags.bBulletProof = false;
                    pVeh->m_nFlags.bExplosionProof = false;
                    pVeh->m_nFlags.bFireProof = false;
                    pVeh->m_nFlags.bCollisionProof = false;
                    pVeh->m_nFlags.bMeleeProof = false;
                    pVeh->m_nFlags.bImmuneToNonPlayerDamage = false;
                    patch::SetRaw(0x614E20, (void*)"\xD9\x9D\x04\x02\x00\x00", 6);

                    // restore tyre burst
                    patch::SetRaw(0x609F30, (void*)"\x53\x56\x57", 3);
                    patch::SetRaw(0x5886A0, (void*)"\x53\x56\x55", 3);
#else
                    pVeh->m_nFlags.bBulletProof = false;
                    pVeh->m_nFlags.bExplosionProof = false;
                    pVeh->m_nFlags.bFireProof = false;
                    pVeh->m_nFlags.bCollisionProof = false;
                    pVeh->m_nFlags.bMeleeProof = false;

                    // restore tyre burst
                    patch::SetRaw(0x53C0E0, (void*)"\x53\x56\x55", 3);
#endif
                }
            }
#ifdef GTASA
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.LockTrainCam"), 0x52A52F, " ", 0xAB, 0x6);
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.LessTraffic"), 0x96917A);
            if (Widget::Toggle(TEXT("Vehicle.NoDerail"), &m_bNoDerail))
            {
                if (m_bNoDerail)
                {
                    patch::Set<uint32_t>(0x6F8C2A, 0x00441F0F, true); // nop dword ptr [eax+eax*1+00h]
                    patch::Set<uint8_t>(0x6F8C2E, 0x00, true);
                    patch::Set<uint16_t>(0x6F8C41, 0xE990, true); // jmp near
                }
                else
                {
                    patch::SetRaw(0x6F8C2A, (void*)"\x8A\x46\x36\xA8\xF8\xD8\x8E", 7);
                }
            }
            // if (Widget::Toggle(TEXT("Vehicle.NoColl"), &m_bDisableColDetection))
            // {
            // 	if (m_bDisableColDetection)
            // 	{
            // 		patch::SetUChar(0x56717B, 0x7D);
            // 		patch::SetUChar(0x56725D, 0x7D);
            // 	}
            // 	// update flags for exising vehicles
            // 	for (auto veh : CPools::ms_pVehiclePool)
            // 	{
            // 		if (veh == FindPlayerVehicle(-1, false))
            // 		{
            // 			continue;
            // 		}
            // 		if (m_bDisableColDetection)
            // 		{
            // 			CCollisionData* pColData = veh->GetColModel()->m_pColData;
            // 			// pColData->m_nNumSpheres = 0;
            // 			pColData->m_nNumBoxes = 0;
            // 			pColData->m_nNumTriangles = 0;
            // 		}
            // 	}
            // }
#endif
            ImGui::NextColumn();
#ifndef GTA3
            if (Widget::Toggle(TEXT("Vehicle.StayOnBike"), &m_bDontFallBike))
            {
                if (m_bDontFallBike)
                {
#ifdef GTASA
                    pPlayer->m_nPedFlags.CantBeKnockedOffBike = 1;
#elif GTAVC

                    patch::PutRetn(0x613920, 0x10);
#endif
                }
                else
                {
#ifdef GTASA
                    pPlayer->m_nPedFlags.CantBeKnockedOffBike = 2;
#elif GTAVC

                    patch::SetRaw(0x613920, (void*)"\x53\x56\x57\x55", 4);
#endif
                }
            }
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.DriveWater"), BY_GAME(0x969152, 0xA10B81, NULL));
#endif
#ifdef GTASA
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.FloatOnHit"), 0x969166);
#endif
#ifndef GTA3
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.GreenLights"), BY_GAME(0x96914E, 0xA10ADC, NULL));
#endif
#ifdef GTASA
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.PerfectHandling"), 0x96914C);
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.TankMode"), 0x969164);

            Widget::Toggle(TEXT("Vehicle.InfNitro"), &VehCustmzr.m_Nitro.m_bEnabled, TEXT("Vehicle.InfNitroTip"));
            if (Widget::Toggle(TEXT("Vehicle.FlipNoBurn"), &m_bVehFlipNoBurn, TEXT("Vehicle.FlipNoBurnTip")))
            {
                // MixSets (Link2012)
                if (m_bVehFlipNoBurn)
                {
                    if (pVeh && pPlayer->m_nPedFlags.bInVehicle)
                    {
                        pVeh->ExtinguishCarFire();
                    }

                    // Patch player vehicle damage when flipped
                    patch::SetRaw(0x570E7F, (void*)"\xD8\xDD\x90\x90\x90\x90", 6); // fstp st0, nop 4
                }
                else
                {
                    // restore patches
                    patch::SetRaw(0x570E7F, (void*)"\xD9\x99\xC0\x04\x00\x00", 6); // fstp dword ptr [ecx+4C0h]
                }
            }

#elif GTA3
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.PerfectHandling"), 0x95CD66);
#endif
            Widget::Toggle(TEXT("Vehicle.Watertight"), &m_bVehWatertight, TEXT("Vehicle.WatertightTip"));
            Widget::ToggleAddr<int8_t>(TEXT("Vehicle.OnlyWheels"), BY_GAME(0x96914B, 0xA10B70, 0x95CD78));
            ImGui::Columns(1);

            if (is_driver)
            {
                ImGui::NewLine();
                ImGui::SeparatorText(TEXT("Vehicle.ForCurVeh"));
                ImGui::Spacing();
                ImGui::Columns(2, 0, false);

                bool state = false;
#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bAlwaysSkidMarks;
                if (Widget::Toggle(TEXT("Vehicle.SkidMarks"), &state, nullptr))
                    pVeh->m_nVehicleFlags.bAlwaysSkidMarks = state;
#endif

                state = BY_GAME(pVeh->m_nPhysicalFlags.bBulletProof, pVeh->m_nFlags.bBulletProof, pVeh->m_nFlags.bBulletProof);
                if (Widget::Toggle(TEXT("Vehicle.BulletProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bBulletProof, pVeh->m_nFlags.bBulletProof, pVeh->m_nFlags.bBulletProof) = state;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof);
                if (Widget::Toggle(TEXT("Vehicle.ColProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof) = state;
                }

#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bDisableParticles;
                if (Widget::Toggle(TEXT("Vehicle.NoParticles"), &state, nullptr))
                {
                    pVeh->m_nVehicleFlags.bDisableParticles = state;
                }

                state = pVeh->m_nVehicleFlags.bVehicleCanBeTargetted;
                if (Widget::Toggle(TEXT("Vehicle.DriverTarget"), &state))
                {
                    pVeh->m_nVehicleFlags.bVehicleCanBeTargetted = state;
                }
#endif

                state = BY_GAME(!pVeh->m_nVehicleFlags.bEngineBroken, true, true) || pVeh->m_nVehicleFlags.bEngineOn;
                if (Widget::Toggle(TEXT("Vehicle.EngineOn"), &state, nullptr, !is_driver))
                {
#ifdef GTASA
                    pVeh->m_nVehicleFlags.bEngineBroken = !state;
#endif
                    pVeh->m_nVehicleFlags.bEngineOn = state;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof);
                if (Widget::Toggle(TEXT("Vehicle.ExplosionProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof) = state;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bFireProof, pVeh->m_nFlags.bFireProof, pVeh->m_nFlags.bFireProof);
                if (Widget::Toggle(TEXT("Vehicle.FireProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bFireProof, pVeh->m_nFlags.bFireProof, pVeh->m_nFlags.bFireProof) = state;
                }

                ImGui::NextColumn();

#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bVehicleCanBeTargettedByHS;
                if (Widget::Toggle(TEXT("Vehicle.HSTarget"), &state, TEXT("Vehicle.HSTargetTip")))
                {
                    pVeh->m_nVehicleFlags.bVehicleCanBeTargettedByHS = state;
                }
#endif

                state = !BY_GAME(pVeh->m_bIsVisible, pVeh->m_nFlags.bIsVisible, pVeh->m_nFlags.bIsVisible);
                if (Widget::Toggle(TEXT("Vehicle.InvisCar"), &state, nullptr, !is_driver))
                {
                    BY_GAME(pVeh->m_bIsVisible, pVeh->m_nFlags.bIsVisible, pVeh->m_nFlags.bIsVisible) = !state;
                }

                state = BY_GAME(!pVeh->ms_forceVehicleLightsOff, pVeh->m_nVehicleFlags.bLightsOn, pVeh->m_nVehicleFlags.bLightsOn);
                if (Widget::Toggle(TEXT("Vehicle.LightsOn"), &state, nullptr, !is_driver))
                {
                    BY_GAME(pVeh->ms_forceVehicleLightsOff, pVeh->m_nVehicleFlags.bLightsOn, pVeh->m_nVehicleFlags.bLightsOn) = state;
                }

                state = pVeh->m_eDoorLock == DOORLOCK_LOCKED_PLAYER_INSIDE;
                if (Widget::Toggle(TEXT("Vehicle.LockDoor"), &state, nullptr, !is_driver))
                {
                    pVeh->m_eDoorLock = state ? DOORLOCK_LOCKED_PLAYER_INSIDE : DOORLOCK_UNLOCKED;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof);
                if (Widget::Toggle(TEXT("Vehicle.MeleeProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof) = state;
                }

#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bPetrolTankIsWeakPoint;
                if (Widget::Toggle(TEXT("Vehicle.PentrolTank"), &state, TEXT("Vehicle.PetrolTankTip")))
                {
                    pVeh->m_nVehicleFlags.bPetrolTankIsWeakPoint = state;
                }

                state = pVeh->m_nVehicleFlags.bSirenOrAlarm;
                if (Widget::Toggle(TEXT("Vehicle.Siren"), &state))
                {
                    pVeh->m_nVehicleFlags.bSirenOrAlarm = state;
                }

                state = pVeh->m_nVehicleFlags.bTakeLessDamage;
                if (Widget::Toggle(TEXT("Vehicle.LessDmg"), &state, nullptr))
                {
                    pVeh->m_nVehicleFlags.bTakeLessDamage = state;
                }
#endif

                ImGui::Columns(1);
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Window.MenusTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("MenusChild");

#ifdef GTASA
            Widget::EditAddr<float>(TEXT("Vehicle.DensityMul"), 0x8A5B20, 0, 1, 10);
#endif
            if (ImGui::CollapsingHeader(TEXT(bPlayerInCar ? "Vehicle.SwitchSeats" : "Vehicle.EnterNearVeh")))
            {
                CVehicle* pTargetVeh = bPlayerInCar ? pVeh : Util::GetClosestCar();

                if (pTargetVeh)
                {
                    int seats = pTargetVeh->m_nMaxPassengers;

                    ImGui::Spacing();
                    ImGui::Columns(2, 0, false);

                    ImGui::Text(Util::GetCarName(pTargetVeh->m_nModelIndex).c_str());
                    ImGui::NextColumn();
                    ImGui::Text(TEXT("Vehicle.TotalSeats"), (seats + 1));
                    ImGui::Columns(1);

                    ImGui::Spacing();
                    if (ImGui::Button(TEXT("Vehicle.Driver"), ImVec2(Widget::CalcSize(2))))
                    {
                        Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, pTargetVeh);
                    }

#ifndef GTA3
                    for (int i = 0; i < seats; ++i)
                    {
                        if (i % 2 != 1)
                        {
                            ImGui::SameLine();
                        }

                        if (ImGui::Button(std::format("{} {}", TEXT("Vehicle.Passenger"), i+1).c_str(),
                                          ImVec2(Widget::CalcSize(2))))
                        {
#ifdef GTASA
                            Command<Commands::WARP_CHAR_INTO_CAR_AS_PASSENGER>(hplayer, pTargetVeh, i);
#elif GTAVC
                            WarpPlayerIntoVehicle(pTargetVeh, i);
#endif
                        }
                    }
#endif
                }
                else
                {
                    ImGui::Spacing();
                    Widget::TextCentered(TEXT("Vehicle.NoNearVeh"));
                }

                ImGui::Spacing();
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader(TEXT("Vehicle.RemoveVehRadius")))
            {
                static int removeRadius;
                ImGui::InputInt(TEXT("Vehicle.Radius"), &removeRadius);
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Vehicle.RemoveVeh"), Widget::CalcSize(1)))
                {
                    CPlayerPed* player = FindPlayerPed();
                    for (CVehicle* pVeh : CPools::ms_pVehiclePool)
                    {
                        if (DistanceBetweenPoints(pVeh->GetPosition(), player->GetPosition()) < removeRadius
                                && player->m_pVehicle != pVeh)
                        {
                            Command<Commands::DELETE_CAR>(CPools::GetVehicleRef(pVeh));
                        }
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
            }

#ifndef GTA3
            std::vector<Widget::BindInfo> color
            {
                {TEXT("Vehicle.Black"), BY_GAME(0x969151, 0xA10B82, NULL)},
                {TEXT("Vehicle.Pink"), BY_GAME(0x969150, 0xA10B26, NULL)}
            };
            Widget::EditRadioBtnAddr(TEXT("Vehicle.TrafficColor"), color);
#endif
#ifdef GTASA
            std::vector<Widget::BindInfo> type
            {
                {TEXT("Vehicle.Cheap"), 0x96915E}, {TEXT("Vehicle.Country"), 0x96917B},
                {TEXT("Vehicle.Fast"), 0x96915F}
            };
            Widget::EditRadioBtnAddr(TEXT("Vehicle.TrafficType"), type);
#endif
            if (pPlayer && pPlayer->m_pVehicle)
            {
                CVehicle* pVeh = pPlayer->m_pVehicle;
                int hVeh = CPools::GetVehicleRef(pVeh);

#ifdef GTASA
                Widget::EditAddr(TEXT("Vehicle.DirtLvl"), (int)pVeh + 0x4B0, 0, 7.5, 15);
                if (pVeh->m_nVehicleClass == VEHICLE_AUTOMOBILE && ImGui::CollapsingHeader(TEXT("Vehicle.Doors")))
                {
                    static int m_nDoorMenuButton;
                    ImGui::Columns(2, 0, false);
                    ImGui::RadioButton(TEXT("Vehicle.Damage"), &m_nDoorMenuButton, 0);
                    ImGui::RadioButton(TEXT("Vehicle.Fix"), &m_nDoorMenuButton, 1);
                    ImGui::NextColumn();
                    ImGui::RadioButton(TEXT("Vehicle.Open"), &m_nDoorMenuButton, 2);
                    ImGui::RadioButton(TEXT("Vehicle.Pop"), &m_nDoorMenuButton, 3);
                    ImGui::Columns(1);
                    ImGui::Spacing();

                    int seats = pVeh->m_nMaxPassengers + 1; // passenger + driver
                    int doors = seats == 4 ? 6 : 4;
                    int hveh = CPools::GetVehicleRef(pVeh);

                    if (ImGui::Button(TEXT("Vehicle.All"), ImVec2(Widget::CalcSize())))
                    {
                        for (int i = 0; i < doors; ++i)
                        {
                            switch (m_nDoorMenuButton)
                            {
                            case 0:
                                Command<Commands::DAMAGE_CAR_DOOR>(hveh, i);
                                break;
                            case 1:
                                Command<Commands::FIX_CAR_DOOR>(hveh, i);
                                break;
                            case 2:
                                Command<Commands::OPEN_CAR_DOOR>(hveh, i);
                                break;
                            case 3:
                                Command<Commands::POP_CAR_DOOR>(hveh, i);
                                break;
                            default:
                                break;
                            }
                        }
                    }

                    for (int i = 0; i != doors; ++i)
                    {
                        if (ImGui::Button(m_DoorNames[i].c_str(), ImVec2(Widget::CalcSize(2))))
                        {
                            switch (m_nDoorMenuButton)
                            {
                            case 0:
                                Command<Commands::DAMAGE_CAR_DOOR>(hveh, i);
                                break;
                            case 1:
                                Command<Commands::FIX_CAR_DOOR>(hveh, i);
                                break;
                            case 2:
                                Command<Commands::OPEN_CAR_DOOR>(hveh, i);
                                break;
                            case 3:
                                Command<Commands::POP_CAR_DOOR>(hveh, i);
                                break;
                            default:
                                break;
                            }
                        }

                        if (i % 2 != 1)
                        {
                            ImGui::SameLine();
                        }
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                }
#endif

                Widget::EditAddr<float>(TEXT("Menu.VehHealth"), (int)&pVeh->m_fHealth, 0, 0, 1000);
                if (ImGui::CollapsingHeader(TEXT("Vehicle.SetSpeed")))
                {
                    Widget::Toggle(TEXT("Vehicle.LockSpeed"), &m_bLockSpeed);
                    ImGui::Spacing();
                    Widget::InputFloat(TEXT("Vehicle.Set"), &m_fLockSpeed, 1.0f, 0.0f, 100.0f);
                    ImGui::Spacing();

                    if (ImGui::Button(TEXT("Vehicle.Set"), ImVec2(Widget::CalcSize(2))))
                    {
                        Util::SetCarForwardSpeed(pVeh, m_fLockSpeed);
                    }

                    ImGui::SameLine();

                    if (ImGui::Button(TEXT("Vehicle.InstantStop"), ImVec2(Widget::CalcSize(2))))
                    {
                        Util::SetCarForwardSpeed(pVeh, 0.0f);
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT( "Window.SpawnTab")))
        {
            ImGui::Spacing();

            ImGui::Columns(BY_GAME(3, 2, 1), NULL, false);
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            static char smodel[8];
            if (ImGui::InputTextWithHint("##SpawnID", TEXT("Vehicle.IDSpawnText"), smodel, 8, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                try
                {
                    int model = std::stoi(smodel);

                    if (CModelInfo::IsVehicleModelType(model) == -1)
                    {
                        Util::SetMessage(TEXT("Vehicle.InvalidID"));
                    }
                    else
                    {
                        std::string str = std::string(smodel);
#ifdef GTASA
                        SpawnVehicle(str);
#else
                        std::string temp = "";
                        SpawnVehicle(temp, temp, str);
#endif
                    }
                }
                catch(...)
                {
                    Util::SetMessage(TEXT("Vehicle.InvalidID"));
                }
            }
#ifdef GTASA
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
            ImGui::InputTextWithHint("##LicenseText", TEXT("Vehicle.PlateText"), m_Spawner.m_nLicenseText, 9);
#endif
#ifndef GTA3
            ImGui::NextColumn();
            static char current_variant = 0;
            
            if (ImGui::ArrowButton("Left", ImGuiDir_Left))
            {
                m_nVehicleVariant -= 1;
                if (m_nVehicleVariant < 0)
                {
                    m_nVehicleVariant = 5;
                }
            }
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
            ImGui::Text(TEXT("Vehicle.Variant"), m_nVehicleVariant);
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::ArrowButton("Right", ImGuiDir_Right))
            {
                m_nVehicleVariant += 1;
                if (m_nVehicleVariant > 4)
                {
                    m_nVehicleVariant = 0;
                }
            }
            
#endif
#ifdef GTASA
            ImGui::Columns(1);
            Widget::ImageList(m_Spawner.m_VehData, fArgWrapper(vehiclePage.SpawnVehicle),
                              [](std::string& str)
            {
                return Util::GetCarName(std::stoi(str));
            }, nullptr, fArgNoneWrapper(vehiclePage.AddNew),
            []()
            {
                if (ImGui::MenuItem(TEXT("Vehicle.SpawnWithTunes"), NULL, &vehiclePage.m_Spawner.m_bWithTunes))
                {
                    gConfig.Set("Features.SpawnWithTunes", vehiclePage.m_Spawner.m_bWithTunes);
                }
                if (ImGui::MenuItem(TEXT("Vehicle.SpawnInAir"), NULL, &vehiclePage.m_Spawner.m_bInAir))
                {
                    gConfig.Set("Features.SpawnAircraftInAir", vehiclePage.m_Spawner.m_bInAir);
                }
                if (ImGui::MenuItem(TEXT("Vehicle.SpawnInside"), NULL, &vehiclePage.m_Spawner.m_bAsDriver))
                {
                    gConfig.Set("Features.SpawnInsideVehicle", vehiclePage.m_Spawner.m_bAsDriver);
                }
            });
#else
            Widget::DataList(m_Spawner.m_VehData, fArg3Wrapper(vehiclePage.SpawnVehicle), fArgNoneWrapper(vehiclePage.AddNew),
                             false,
                             []()
            {
                if (ImGui::MenuItem(TEXT("Vehicle.SpawnInAir"), NULL, &vehiclePage.m_Spawner.m_bInAir))
                {
                    gConfig.Set("Features.SpawnAircraftInAir", vehiclePage.m_Spawner.m_bInAir);
                }
                if (ImGui::MenuItem(TEXT("Vehicle.SpawnInside"), NULL, &vehiclePage.m_Spawner.m_bAsDriver))
                {
                    gConfig.Set("Features.SpawnInsideVehicle", vehiclePage.m_Spawner.m_bAsDriver);
                }
            });
#endif
            ImGui::EndTabItem();
        }
        if (pPlayer->m_pVehicle && bPlayerInCar)
        {
            CVehicle* veh = FindPlayerPed()->m_pVehicle;
            int hveh = CPools::GetVehicleRef(veh);
#ifdef GTASA
            if (ImGui::BeginTabItem(TEXT( "Vehicle.Customize")))
            {
                ImGui::Spacing();

                if (ImGui::BeginTabBar("CustomizeTab"))
                {
                    VehCustmzr.Draw();
                    ImGui::EndTabBar();
                }
                ImGui::EndTabItem();
            }
#else
            VehCustmzr.Draw();
#endif
            if (ImGui::BeginTabItem(TEXT( "Vehicle.AutoDrive")))
            {
                ImGui::Spacing();
                AutoDrive.Draw();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}