#include "pch.h"
#include "vehicle.h"
#include "menu.h"
#include "widget.h"
#include "util.h"
#include "teleport.h"
#include "filehandler.h"
#include <CPopulation.h>
#include <CDamageManager.h>

#ifdef GTASA
#include "tHandlingData.h"
#include "neon.h"
#include "paint.h"
#endif

void Vehicle::Init()
{
#ifdef GTASA
    FileHandler::FetchHandlingID(m_VehicleIDE);
    Neon::InjectHooks();
    Paint::InjectHooks();
#endif

    FileHandler::FetchColorData(m_CarcolsColorData);

    // Get config data
    Spawner::m_bSpawnInAir = gConfig.Get("Features.SpawnAircraftInAir", true);
    Spawner::m_bSpawnInside = gConfig.Get("Features.SpawnInsideVehicle", true);

    Events::processScriptsEvent += []
    {
        uint timer = CTimer::m_snTimeInMilliseconds;
        CPlayerPed* pPlayer = FindPlayerPed();
        CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());

        if (pPlayer && Util::IsInVehicle())
        {
            int hveh = CPools::GetVehicleRef(pVeh);
            float speed = pVeh->m_vecMoveSpeed.Magnitude() * 50.0f;
            if (m_bAutoUnflip && pVeh->IsUpsideDown() && speed < 2.0f)
            {
                Util::UnFlipVehicle(pVeh);
            }

            if (unflipVeh.Pressed())
            {
                Util::UnFlipVehicle(pVeh);
            }

            if (fixVeh.Pressed())
            {
                Util::FixVehicle(pVeh);
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
                pVeh->m_nVehicleFlags.bCanBeDamaged = true;
#elif GTAVC
                pVeh->m_nFlags.bBulletProof = true;
                pVeh->m_nFlags.bExplosionProof = true;
                pVeh->m_nFlags.bFireProof = true;
                pVeh->m_nFlags.bCollisionProof = true;
                pVeh->m_nFlags.bMeleeProof = true;
                pVeh->m_nFlags.bImmuneToNonPlayerDamage = true;
#else
                pVeh->m_nFlags.bBulletProof = true;
                pVeh->m_nFlags.bExplosionProof = true;
                pVeh->m_nFlags.bFireProof = true;
                pVeh->m_nFlags.bCollisionProof = true;
                pVeh->m_nFlags.bMeleeProof = true;
#endif
            }

            Command<Commands::SET_CAR_HEAVY>(hveh, m_bVehHeavy);
            Command<Commands::SET_CAR_WATERTIGHT>(hveh, m_bVehWatertight);

            if (m_bLockSpeed)
            {
                Util::SetCarForwardSpeed(pVeh, m_fLockSpeed);
            }

#ifdef GTASA
            if (UnlimitedNitro::m_bEnabled && pVeh->m_nVehicleSubClass == VEHICLE_AUTOMOBILE)
            {
                patch::Set<BYTE>(0x969165, 0, true); // All cars have nitro
                patch::Set<BYTE>(0x96918B, 0, true); // All taxis have nitro

                if (KeyPressed(VK_LBUTTON))
                {
                    if (!UnlimitedNitro::m_bCompAdded)
                    {
                        AddComponent("1010", false);
                        UnlimitedNitro::m_bCompAdded = true;
                    }
                }
                else
                {
                    if (UnlimitedNitro::m_bCompAdded)
                    {
                        RemoveComponent("1010", false);
                        UnlimitedNitro::m_bCompAdded = false;
                    }
                }
            }

            if (NeonData::m_bRainbowEffect && timer - NeonData::m_nRainbowTimer > 50)
            {
                int red, green, blue;

                Util::RainbowValues(red, green, blue, 0.25);
                Neon::Install(pVeh, red, green, blue);
                NeonData::m_nRainbowTimer = timer;
            }
#endif
        }

#ifdef GTASA
        // Traffic neons
        if (NeonData::m_bApplyOnTraffic && timer - NeonData::m_bTrafficTimer > 1000)
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

                if (chance == 1 && !Neon::IsInstalled(veh) && veh->m_pDriver != pPlayer)
                {
                    Neon::Install(veh, Random(0, 255), Random(0, 255), Random(0, 255));
                }
            }
            NeonData::m_bTrafficTimer = timer;
        }

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
void Vehicle::AddComponent(const std::string& component, const bool display_message)
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


void Vehicle::RemoveComponent(const std::string& component, const bool display_message)
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

// hardcoded for now
int Vehicle::GetRandomTrainIdForModel(int model)
{
    static int train_ids[] =
    {
        8, 9, // model 449
        0, 3, 6, 10, 12, 13, // model 537
        1, 5, 15 // model 538
    };
    int _start = 0, _end = 0;

    switch (model)
    {
    case 449:
        _start = 0;
        _end = 1;
        break;
    case 537:
        _start = 2;
        _end = 7;
        break;
    case 538:
        _start = 8;
        _end = 10;
        break;
    default:
        Util::SetMessage("Invalid train model");
        return -1;
    }
    int id = Random(_start, _end);
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

#ifdef GTASA
void Vehicle::SpawnVehicle(std::string& smodel)
#else
void Vehicle::SpawnVehicle(std::string& rootkey, std::string& vehName, std::string& smodel)
#endif
{
    CPlayerPed* player = FindPlayerPed();
    int hplayer = CPools::GetPedRef(player);
    int imodel = std::stoi(smodel);
    CVehicle* veh = nullptr;
    int interior = BY_GAME(player->m_nAreaCode, player->m_nAreaCode, NULL);

    CVector pos = player->GetPosition();
    float speed = 0;

    bool bInVehicle = Command<Commands::IS_CHAR_IN_ANY_CAR>(hplayer);
    if (bInVehicle && Spawner::m_bSpawnInside)
    {
        CVehicle* pveh = player->m_pVehicle;
        int hveh = CPools::GetVehicleRef(pveh);
        pos = pveh->GetPosition();

        Command<Commands::GET_CAR_SPEED>(hveh, &speed);
        Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(hplayer, pos.x, pos.y, pos.z);

#ifdef GTASA
        if (pveh->m_nVehicleClass == VEHICLE_TRAIN)
        {
            Command<Commands::DELETE_MISSION_TRAIN>(hveh);
        }
        else
        {
            
            Command<Commands::DELETE_CAR>(hveh);
        }
#else
        Command<Commands::DELETE_CAR>(hveh);
#endif
    }

    if (interior == 0)
    {
        if (Spawner::m_bSpawnInAir && (CModelInfo::IsHeliModel(imodel) || CModelInfo::IsPlaneModel(imodel)))
        {
            pos.z = 400;
        }
        else
        {
            pos.z -= 5;
        }
    }

#ifdef GTASA
    if (CModelInfo::IsTrainModel(imodel))
    {
        int train_id = GetRandomTrainIdForModel(imodel);

        if (train_id == -1) // Unknown train id
            return;

        int hveh = 0;

        // Loading all train related models
        CStreaming::RequestModel(590, PRIORITY_REQUEST);
        CStreaming::RequestModel(538, PRIORITY_REQUEST);
        CStreaming::RequestModel(570, PRIORITY_REQUEST);
        CStreaming::RequestModel(569, PRIORITY_REQUEST);
        CStreaming::RequestModel(537, PRIORITY_REQUEST);
        CStreaming::RequestModel(449, PRIORITY_REQUEST);

        CStreaming::LoadAllRequestedModels(false);

        CTrain* train = nullptr;
        CTrain* carraige = nullptr;
        int track = Random(0, 1);
        int node = CTrain::FindClosestTrackNode(pos, &track);
        CTrain::CreateMissionTrain(pos, (Random(0, 1)) == 1 ? true : false, train_id, &train, &carraige, node,
                                   track, false);

        veh = (CVehicle*)train;
        hveh = CPools::GetVehicleRef(veh);
        if (veh->m_pDriver)
            Command<Commands::DELETE_CHAR>(CPools::GetPedRef(veh->m_pDriver));

        if (Spawner::m_bSpawnInside)
        {
            Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hveh);
            Util::SetCarForwardSpeed(veh, speed);
        }
        Command<Commands::MARK_MISSION_TRAIN_AS_NO_LONGER_NEEDED>(hveh);
        Command<Commands::MARK_CAR_AS_NO_LONGER_NEEDED>(hveh);
        CStreaming::SetModelIsDeletable(590);
        CStreaming::SetModelIsDeletable(538);
        CStreaming::SetModelIsDeletable(570);
        CStreaming::SetModelIsDeletable(569);
        CStreaming::SetModelIsDeletable(537);
        CStreaming::SetModelIsDeletable(449);
    }
    else
    {
#endif
        CStreaming::RequestModel(imodel, PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(false);
#ifdef GTASA
        if (Spawner::m_nLicenseText[0] != '\0')
        {
            Command<Commands::CUSTOM_PLATE_FOR_NEXT_CAR>(imodel, Spawner::m_nLicenseText);
        }
#endif
        int hveh = 0;
        if (Spawner::m_bSpawnInside)
        {
            Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 4.0f, &hveh);
            veh = CPools::GetVehicle(hveh);
#ifdef GTASA
            veh->SetHeading(player->GetHeading());
#elif GTAVC
            float x,y,z;
            player->m_placement.GetOrientation(x, y, z);
            veh->m_placement.SetOrientation(x, y, z);
#else
            float x,y,z;
            player->GetOrientation(x, y, z);
            veh->SetOrientation(x, y, z);
#endif
            Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hveh);

            Util::SetCarForwardSpeed(veh, speed);
        }
        else
        {
#ifdef GTASA
            player->TransformFromObjectSpace(pos, CVector(0, 10, 0));
#else
            player->TransformFromObjectSpace(pos);
#endif
            Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 3.0f, &hveh);
            veh = CPools::GetVehicle(hveh);
#ifdef GTASA
            veh->SetHeading(player->GetHeading() + 55.0f);
#elif GTAVC
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
#ifndef GTA3
        BY_GAME(veh->m_nAreaCode, veh->m_nAreaCode, NULL) = interior;
#endif
        Command<Commands::MARK_CAR_AS_NO_LONGER_NEEDED>(CPools::GetVehicleRef(veh));
        CStreaming::SetModelIsDeletable(imodel);
#ifdef GTASA
    }
    veh->m_nVehicleFlags.bHasBeenOwnedByPlayer = true;
#else
        Command<Commands::RESTORE_CAMERA_JUMPCUT>();
#endif
}

std::string Vehicle::GetNameFromModel(int model)
{
#ifdef GTA3
    return std::to_string(model);
#else
    return (const char*)CModelInfo::GetModelInfo(model) + 0x32;
#endif
}

int Vehicle::GetModelFromName(const char* name)
{
    int model = 0;
    CBaseModelInfo* pModelInfo = CModelInfo::GetModelInfo((char*)name, &model);

    if (model > 0 && model < 1000000 && GetNameFromModel(model) != "")
    {
        return model;
    }
    else
    {
        return 0;
    }
}

static void StartAutoDrive(CVehicle *pVeh, const char *buf = nullptr)
{
    int hVeh = CPools::GetVehicleRef(pVeh);
    CVector pos;

    if (buf ==  nullptr)
    {
#ifdef GTASA
        tRadarTrace targetBlip = CRadar::ms_RadarTrace[LOWORD(FrontEndMenuManager.m_nTargetBlipIndex)];
        pos = targetBlip.m_vecPos;
        if (targetBlip.m_nRadarSprite != RADAR_SPRITE_WAYPOINT)
        {
            Util::SetMessage(TEXT("Teleport.TargetBlipText"));
            return;
        }
#else
        return;
#endif
    }
    else
    {
         if (sscanf(buf, "%f,%f,%f", &pos.x, &pos.y, &pos.z) != 3)
        {
            int dim;
            sscanf(buf, "%d,%f,%f,%f", &dim, &pos.x, &pos.y, &pos.z);
        }
    }

    int model = pVeh->m_nModelIndex;
    if (CModelInfo::IsBoatModel(model))
    {
        Command<Commands::BOAT_GOTO_COORDS>(hVeh, pos.x, pos.y, pos.z);
    }
    else if (CModelInfo::IsPlaneModel(model))
    {
        CVector p = pVeh->GetPosition();
        p.z = 300.0f;
#ifdef GTASA
        pVeh->SetPosn(p);
#elif GTAVC
        pVeh->SetPosition(p);
#else   
        pVeh->SetPos(p);
#endif
        Command<Commands::PLANE_GOTO_COORDS>(hVeh, pos.x, pos.y, 300.0f, 30, 200);
    }
    else if (CModelInfo::IsHeliModel(model))
    {
        CVector p = pVeh->GetPosition();
        p.z = 300.0f;
#ifdef GTASA
        pVeh->SetPosn(p);
#elif GTAVC
        pVeh->SetPosition(p);
#else   
        pVeh->SetPos(p);
#endif
        Command<Commands::HELI_GOTO_COORDS>(hVeh, pos.x, pos.y, 300.0f, 30, 200);
    }
#ifdef GTASA
    else if (CModelInfo::IsTrainModel(model))
    {
        return;
    }
#endif
    else
    {
        Command<Commands::CAR_GOTO_COORDINATES>(hVeh, pos.x, pos.y, pos.z);
    }
}

void Vehicle::ShowPage()
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

    if (ImGui::Button(TEXT("Vehicle.FixCar"), ImVec2(Widget::CalcSize(3))) && Util::IsInVehicle())
    {
        Util::FixVehicle(pVeh);
    }

    ImGui::SameLine();

    if (ImGui::Button(TEXT("Vehicle.FlipCar"), ImVec2(Widget::CalcSize(3))) && Util::IsInVehicle())
    {
        Util::UnFlipVehicle(pVeh);
    }

    ImGui::Spacing();
   
    if (ImGui::BeginTabBar("Vehicle", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
    { 
        CVehicle* pVeh = pPlayer->m_pVehicle;
        bool is_driver = pVeh && (pPlayer->m_pVehicle->m_pDriver == pPlayer);

        ImGui::Spacing();

        if (ImGui::BeginTabItem(TEXT("Window.CheckboxTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("CheckboxesChild");
            ImGui::Columns(2, 0, false);
#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Vehicle.AimDrive"), 0x969179);
            Widget::CheckboxAddr(TEXT("Vehicle.AllNitro"), 0x969165);
#endif

            Widget::Checkbox(TEXT("Vehicle.AutoUnflip"), &m_bAutoUnflip);

#ifndef GTA3
            Widget::CheckboxAddr(TEXT("Vehicle.AggroDriver"), BY_GAME(0x96914F,0xA10B47, NULL));
            Widget::CheckboxAddr(TEXT("Vehicle.AllTaxiNitro"), BY_GAME(0x96918B,0xA10B3A, NULL));
            Widget::Checkbox(TEXT("Vehicle.BikeFly"), &m_bBikeFly);
            Widget::CheckboxAddr(TEXT("Vehicle.BoatFly"), BY_GAME(0x969153, 0xA10B11, NULL));
#endif
            Widget::CheckboxAddr(TEXT("Vehicle.CarFly"), BY_GAME(0x969160, 0xA10B28, 0x95CD75));
            Widget::Checkbox(TEXT("Vehicle.CarHeavy"), &m_bVehHeavy);
            if (Widget::Checkbox(TEXT("Vehicle.DmgProof"), &m_bNoDamage, TEXT("Vehicle.DmgProofTip")))
            {
                if (pVeh && !m_bNoDamage)
                {
#ifdef GTASA
                    pVeh->m_nPhysicalFlags.bBulletProof = false;
                    pVeh->m_nPhysicalFlags.bExplosionProof = false;
                    pVeh->m_nPhysicalFlags.bFireProof = false;
                    pVeh->m_nPhysicalFlags.bCollisionProof = false;
                    pVeh->m_nPhysicalFlags.bMeleeProof = false;
                    pVeh->m_nVehicleFlags.bCanBeDamaged = false;
#elif GTAVC
                    pVeh->m_nFlags.bBulletProof = false;
                    pVeh->m_nFlags.bExplosionProof = false;
                    pVeh->m_nFlags.bFireProof = false;
                    pVeh->m_nFlags.bCollisionProof = false;
                    pVeh->m_nFlags.bMeleeProof = false;
                    pVeh->m_nFlags.bImmuneToNonPlayerDamage = false;
#else
                    pVeh->m_nFlags.bBulletProof = false;
                    pVeh->m_nFlags.bExplosionProof = false;
                    pVeh->m_nFlags.bFireProof = false;
                    pVeh->m_nFlags.bCollisionProof = false;
                    pVeh->m_nFlags.bMeleeProof = false;
#endif
                }
            }
#ifdef GTASA
            Widget::CheckboxAddrRaw(TEXT("Vehicle.LockTrainCam"), 0x52A52F, 1, "\xAB", "\x06");
            Widget::CheckboxAddr(TEXT("Vehicle.LessTraffic"), 0x96917A);
            if (Widget::Checkbox(TEXT("Vehicle.NoDerail"), &m_bNoDerail))
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
            // if (Widget::Checkbox(TEXT("Vehicle.NoColl"), &m_bDisableColDetection))
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
            if (Widget::Checkbox(TEXT("Vehicle.StayOnBike"), &m_bDontFallBike))
            {
                if (m_bDontFallBike)
                {
#ifdef GTASA
                    pPlayer->m_nPedFlags.CantBeKnockedOffBike = 1;
#elif GTAVC
                    
                    patch::SetRaw(0x614C4E, (void*)"\x8B\x8D\x00\x00\x00\x00", 6);
                    patch::SetRaw(0x614CC5, (void*)"\x8B\x85\x00\x00\x00\x00", 6);
#endif
                }
                else
                {
#ifdef GTASA
                    pPlayer->m_nPedFlags.CantBeKnockedOffBike = 2;
#elif GTAVC
                    
                    patch::SetRaw(0x614C4E, (void*)"\x8B\x8D\xA8\x01\x00\x00", 6);
                    patch::SetRaw(0x614CC5, (void*)"\x8B\x85\xAC\x01\x00\x00", 6);
#endif
                }
            }
            Widget::CheckboxAddr(TEXT("Vehicle.DriveWater"), BY_GAME(0x969152, 0xA10B81, NULL));
#endif
#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Vehicle.FloatOnHit"), 0x969166);
#endif
#ifndef GTA3
            Widget::CheckboxAddr(TEXT("Vehicle.GreenLights"), BY_GAME(0x96914E, 0xA10ADC, NULL));
#endif
#ifdef GTASA
            Widget::CheckboxAddr(TEXT("Vehicle.PerfectHandling"), 0x96914C);
            Widget::CheckboxAddr(TEXT("Vehicle.TankMode"), 0x969164);

            Widget::Checkbox(TEXT("Vehicle.InfNitro"), &UnlimitedNitro::m_bEnabled, TEXT("Vehicle.InfNitroTip"));
            if (Widget::Checkbox(TEXT("Vehicle.FlipNoBurn"), &m_bVehFlipNoBurn, TEXT("Vehicle.FlipNoBurnTip")))
            {
                // MixSets (Link2012)
                if (m_bVehFlipNoBurn)
                {
                    // Patch ped vehicles damage when flipped
                    patch::SetRaw(0x6A776B, (void*)"\xD8\xDD\x00\x00\x00\x00", 6); // fstp st0, nop 4

                    // Patch player vehicle damage when flipped
                    patch::SetRaw(0x570E7F, (void*)"\xD8\xDD\x00\x00\x00\x00", 6); // fstp st0, nop 4
                }
                else
                {
                    // restore patches
                    patch::SetRaw(0x6A776B, (void*)"\xD9\x9E\xC0\x04\x00\x00", 6);
                    patch::SetRaw(0x570E7F, (void*)"\xD9\x99\xC0\x04\x00\x00", 6); // fstp dword ptr [ecx+4C0h]
                }
            }

#elif GTA3
            Widget::CheckboxAddr(TEXT("Vehicle.PerfectHandling"), 0x95CD66);
#endif
            Widget::Checkbox(TEXT("Vehicle.Watertight"), &m_bVehWatertight, TEXT("Vehicle.WatertightTip"));
            Widget::CheckboxAddr(TEXT("Vehicle.OnlyWheels"), BY_GAME(0x96914B, 0xA10B70, 0x95CD78));
            ImGui::Columns(1);

            if (is_driver)
            {
                ImGui::NewLine();
                ImGui::TextWrapped(TEXT("Vehicle.ForCurVeh"));

                ImGui::Columns(2, 0, false);

                bool state = false;
#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bAlwaysSkidMarks;
                if (Widget::Checkbox(TEXT("Vehicle.SkidMarks"), &state, nullptr))
                    pVeh->m_nVehicleFlags.bAlwaysSkidMarks = state;
#endif

                state = BY_GAME(pVeh->m_nPhysicalFlags.bBulletProof, pVeh->m_nFlags.bBulletProof, pVeh->m_nFlags.bBulletProof);
                if (Widget::Checkbox(TEXT("Vehicle.BulletProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bBulletProof, pVeh->m_nFlags.bBulletProof, pVeh->m_nFlags.bBulletProof) = state;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof);
                if (Widget::Checkbox(TEXT("Vehicle.ColProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof, pVeh->m_nFlags.bCollisionProof) = state;
                }

#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bDisableParticles;
                if (Widget::Checkbox(TEXT("Vehicle.NoParticles"), &state, nullptr))
                {
                    pVeh->m_nVehicleFlags.bDisableParticles = state;
                }

                state = pVeh->m_nVehicleFlags.bVehicleCanBeTargetted;
                if (Widget::Checkbox(TEXT("Vehicle.DriverTarget"), &state))
                {
                    pVeh->m_nVehicleFlags.bVehicleCanBeTargetted = state;
                }
#endif

                state = BY_GAME(!pVeh->m_nVehicleFlags.bEngineBroken, true, true) || pVeh->m_nVehicleFlags.bEngineOn;
                if (Widget::Checkbox(TEXT("Vehicle.EngineOn"), &state, nullptr, !is_driver))
                {
#ifdef GTASA
                    pVeh->m_nVehicleFlags.bEngineBroken = !state;
#endif
                    pVeh->m_nVehicleFlags.bEngineOn = state;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof);
                if (Widget::Checkbox(TEXT("Vehicle.ExplosionProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof, pVeh->m_nFlags.bExplosionProof) = state;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bFireProof, pVeh->m_nFlags.bFireProof, pVeh->m_nFlags.bFireProof);
                if (Widget::Checkbox(TEXT("Vehicle.FireProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bFireProof, pVeh->m_nFlags.bFireProof, pVeh->m_nFlags.bFireProof) = state;
                }

                ImGui::NextColumn();

#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bVehicleCanBeTargettedByHS;
                if (Widget::Checkbox(TEXT("Vehicle.HSTarget"), &state, TEXT("Vehicle.HSTargetTip")))
                {
                    pVeh->m_nVehicleFlags.bVehicleCanBeTargettedByHS = state;
                }
#endif

                state = !BY_GAME(pVeh->m_bIsVisible, pVeh->m_nFlags.bIsVisible, pVeh->m_nFlags.bIsVisible);
                if (Widget::Checkbox(TEXT("Vehicle.InvisCar"), &state, nullptr, !is_driver))
                {
                    BY_GAME(pVeh->m_bIsVisible, pVeh->m_nFlags.bIsVisible, pVeh->m_nFlags.bIsVisible) = !state;
                }

                state = BY_GAME(!pVeh->ms_forceVehicleLightsOff, pVeh->m_nVehicleFlags.bLightsOn, pVeh->m_nVehicleFlags.bLightsOn);
                if (Widget::Checkbox(TEXT("Vehicle.LightsOn"), &state, nullptr, !is_driver))
                {
                    BY_GAME(pVeh->ms_forceVehicleLightsOff, pVeh->m_nVehicleFlags.bLightsOn, pVeh->m_nVehicleFlags.bLightsOn) = state;
                }

                state = pVeh->m_eDoorLock == DOORLOCK_LOCKED_PLAYER_INSIDE;
                if (Widget::Checkbox(TEXT("Vehicle.LockDoor"), &state, nullptr, !is_driver))
                {
                    pVeh->m_eDoorLock = state ? DOORLOCK_LOCKED_PLAYER_INSIDE : DOORLOCK_UNLOCKED;
                }

                state = BY_GAME(pVeh->m_nPhysicalFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof);
                if (Widget::Checkbox(TEXT("Vehicle.MeleeProof"), &state, nullptr, m_bNoDamage))
                {
                    BY_GAME(pVeh->m_nPhysicalFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof, pVeh->m_nFlags.bMeleeProof) = state;
                }

#ifdef GTASA
                state = pVeh->m_nVehicleFlags.bPetrolTankIsWeakPoint;
                if (Widget::Checkbox(TEXT("Vehicle.PentrolTank"), &state, TEXT("Vehicle.PetrolTankTip")))
                {
                    pVeh->m_nVehicleFlags.bPetrolTankIsWeakPoint = state;
                }

                state = pVeh->m_nVehicleFlags.bSirenOrAlarm;
                if (Widget::Checkbox(TEXT("Vehicle.Siren"), &state))
                {
                    pVeh->m_nVehicleFlags.bSirenOrAlarm = state;
                }

                state = pVeh->m_nVehicleFlags.bTakeLessDamage;
                if (Widget::Checkbox(TEXT("Vehicle.LessDmg"), &state, nullptr))
                {
                    pVeh->m_nVehicleFlags.bTakeLessDamage = state;
                }
#endif

                ImGui::Columns(1);
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TEXT("Window.MenusTab")))
        {
            ImGui::Spacing();
            ImGui::BeginChild("MenusChild");

#ifdef GTASA
            Widget::EditAddr<float>(TEXT("Vehicle.DensityMul"), 0x8A5B20, 0, 1, 10);
#endif
            if (ImGui::CollapsingHeader(TEXT(bPlayerInCar ? "Vehicle.SwitchSeats" : "Vehicle.EnterNearVeh")))
            {
                CVehicle* pTargetVeh = bPlayerInCar ? pVeh : Util::GetClosestVehicle();

                if (pTargetVeh)
                {
                    int seats = pTargetVeh->m_nMaxPassengers;

                    ImGui::Spacing();
                    ImGui::Columns(2, 0, false);

                    ImGui::Text(GetNameFromModel(pTargetVeh->m_nModelIndex).c_str());
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
                ImGui::InputInt(TEXT("Vehicle.Radius"), &m_nVehRemoveRadius);
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Vehicle.RemoveVeh"), Widget::CalcSize(1)))
                {
                    CPlayerPed* player = FindPlayerPed();
                    for (CVehicle* pVeh : CPools::ms_pVehiclePool)
                    {
                        if (DistanceBetweenPoints(pVeh->GetPosition(), player->GetPosition()) < m_nVehRemoveRadius
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
                    Widget::Checkbox(TEXT("Vehicle.LockSpeed"), &m_bLockSpeed);
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
        if (ImGui::BeginTabItem(TEXT("Window.SpawnTab")))
        {
            ImGui::Spacing();
            ImGui::Columns(2, 0, false);
            if (Widget::Checkbox(TEXT("Vehicle.SpawnInside"), &Spawner::m_bSpawnInside))
            {
                gConfig.Set("Features.SpawnInsideVehicle", Spawner::m_bSpawnInside);
            }
            ImGui::NextColumn();
            if( Widget::Checkbox(TEXT("Vehicle.SpawnInAir"), &Spawner::m_bSpawnInAir))
            {
                gConfig.Set("Features.SpawnAircraftInAir", Spawner::m_bSpawnInAir);
            }
            ImGui::Columns(1);

            ImGui::Spacing();

            int width = ImGui::GetWindowContentRegionWidth() - ImGui::GetStyle().ItemSpacing.x;
#ifdef GTASA
            width /= 2;
#endif

            ImGui::SetNextItemWidth(width);
            static char smodel[8];
            if (ImGui::InputTextWithHint("##SpawnID", TEXT("Vehicle.IDSpawnText"), smodel, 8, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                try{
                    int model = std::stoi(smodel);

                    if (CModelInfo::IsCarModel(model))
                    {
                        std::string str = std::string(smodel);
#ifdef GTASA
                        SpawnVehicle(str);
#else
                        std::string temp = "";
                        SpawnVehicle(temp, temp, str);
#endif
                    }
                    else
                    {
                        Util::SetMessage(TEXT("Vehicle.InvalidID"));
                    }
                }
                catch(...)
                {
                    Util::SetMessage(TEXT("Vehicle.InvalidID"));
                }
            }
#ifdef GTASA
            ImGui::SameLine();
            ImGui::SetNextItemWidth(width);
            ImGui::InputTextWithHint("##LicenseText", TEXT("Vehicle.PlateText"), Spawner::m_nLicenseText, 9);

            Widget::ImageList(Spawner::m_VehData, SpawnVehicle,
            [](std::string& str)
            {
                return GetNameFromModel(std::stoi(str));
            });
#else
            Widget::DataList(Spawner::m_VehData, SpawnVehicle, nullptr);
#endif
            ImGui::EndTabItem();
        }
        if (pPlayer->m_pVehicle && bPlayerInCar)
        {
            CVehicle* veh = FindPlayerPed()->m_pVehicle;
            int hveh = CPools::GetVehicleRef(veh);
            if (ImGui::BeginTabItem(TEXT("Vehicle.AutoDrive")))
            {
                ImGui::Spacing();
                if (ImGui::Button(TEXT("Vehicle.AutoDriveStop"), Widget::CalcSize(1)))
                {
                    Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hveh);
                }
                ImGui::Spacing();
                if (ImGui::BeginTabBar("PassTabaBar"))
                {
                    if (ImGui::BeginTabItem(TEXT("Teleport.Coordinates")))
                    {
                        static char buf[INPUT_BUFFER_SIZE];
                        ImGui::Spacing();
                        ImGui::TextWrapped(TEXT("Vehicle.AutoDriveInfo"));
                        ImGui::Spacing();
                        ImGui::InputTextWithHint(TEXT("Teleport.Coordinates"), "x, y, z", buf, IM_ARRAYSIZE(buf));
                        ImGui::Spacing();
                        if (ImGui::Button(TEXT("Vehicle.AutoDriveCoord"), Widget::CalcSize(BY_GAME(2,1,1))))
                        {
                            StartAutoDrive(pVeh, buf);
                        }
#ifdef GTASA
                        ImGui::SameLine();
                        if (ImGui::Button(TEXT("Vehicle.AutoDriveMarker"), Widget::CalcSize(2)))
                        {
                            StartAutoDrive(pVeh);
                        }
#endif
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(TEXT("Teleport.Location")))
                    {
                        ImGui::Spacing();
                        Widget::DataList(Teleport::m_locData, 
                        [](std::string& rootkey, std::string& bLocName, std::string& loc)
                        {
                            CVehicle* pVeh = BY_GAME(FindPlayerVehicle(-1, false), FindPlayerVehicle(), FindPlayerVehicle());
                            StartAutoDrive(pVeh, loc.c_str());       
                        }, nullptr);
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem(TEXT("Vehicle.Color")))
            {
#ifdef GTASA
                Paint::GenerateNodeList(veh, PaintData::m_vecNames, PaintData::m_Selected);

                ImGui::Spacing();
                if (ImGui::Button(TEXT("Vehicle.ResetColor"), ImVec2(Widget::CalcSize())))
                {
                    Paint::ResetNodeColor(veh, PaintData::m_Selected);
                    Util::SetMessage(TEXT("Vehicle.ResetColorMSG"));
                }
                ImGui::Spacing();

                Widget::ListBox(TEXT("Vehicle.Component"), PaintData::m_vecNames, PaintData::m_Selected);

                if (ImGui::ColorEdit3(TEXT("Vehicle.ColorPicker"), PaintData::m_fColorPicker))
                {
                    uchar r = PaintData::m_fColorPicker[0] * 255;
                    uchar g = PaintData::m_fColorPicker[1] * 255;
                    uchar b = PaintData::m_fColorPicker[2] * 255;
                    Paint::SetNodeColor(veh, PaintData::m_Selected, { r, g, b, 255 }, PaintData::m_bMatFilter);
                }
#endif

                ImGui::Spacing();
                ImGui::Columns(2, NULL, false);

#ifdef GTASA
                ImGui::Checkbox(TEXT("Vehicle.MatFilter"), &PaintData::m_bMatFilter);
                ImGui::RadioButton(TEXT("Vehicle.Primary"), &PaintData::m_nRadioButton, 1);
                ImGui::RadioButton(TEXT("Vehicle.Secondary"), &PaintData::m_nRadioButton, 2);
                ImGui::NextColumn();
                ImGui::NewLine();
                ImGui::RadioButton(TEXT("Vehicle.Tertiary"), &PaintData::m_nRadioButton, 3);
                ImGui::RadioButton(TEXT("Vehicle.Quaternary"), &PaintData::m_nRadioButton, 4);
#else
                ImGui::RadioButton(TEXT("Vehicle.Primary"), &PaintData::m_nRadioButton, 1);
                ImGui::NextColumn();
                ImGui::RadioButton(TEXT("Vehicle.Secondary"), &PaintData::m_nRadioButton, 2);
#endif
                ImGui::Spacing();
                ImGui::Columns(1);
                ImGui::Text(TEXT("Vehicle.SelectPreset"));
                ImGui::Spacing();

                int count = (int)m_CarcolsColorData.size();

                ImVec2 size = Widget::CalcSize();
                int btnsInRow = ImGui::GetWindowContentRegionWidth() / (size.y * 2);
                int btnSize = (ImGui::GetWindowContentRegionWidth() - int(ImGuiStyleVar_ItemSpacing) * (btnsInRow -
                               0.6 * btnsInRow)) / btnsInRow;

                ImGui::BeginChild("Colorss");

                for (int colorId = 0; colorId < count; ++colorId)
                {
                    if (Widget::ColorBtn(colorId, m_CarcolsColorData[colorId], ImVec2(btnSize, btnSize)))
                    {
                        *(uint8_replacement*)(int(veh) + BY_GAME(0x433, 0x19F, 0x19B) + PaintData::m_nRadioButton) = colorId;
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
            if (gRenderer != Render_DirectX11)
            {
                if (ImGui::BeginTabItem(TEXT("Vehicle.NeonsTab")))
                {
                    int model = veh->m_nModelIndex;
                    ImGui::Spacing();
                    if (ImGui::Button(TEXT("Vehicle.RemoveNeon"), ImVec2(Widget::CalcSize())))
                    {
                        Neon::Remove(veh);
                        Util::SetMessage(TEXT("Vehicle.RemoveNeonMSG"));
                    }

                    ImGui::Spacing();
                    ImGui::Columns(2, NULL, false);

                    bool pulsing = Neon::IsPulsingEnabled(veh);
                    if (Widget::Checkbox(TEXT("Vehicle.PulsingNeon"), &pulsing))
                    {
                        Neon::SetPulsing(veh, pulsing);
                    }

                    Widget::Checkbox(TEXT("Vehicle.RainbowNeon"), &NeonData::m_bRainbowEffect, TEXT("Vehicle.RainbowNeonMSG"));
                    ImGui::NextColumn();
                    Widget::Checkbox(TEXT("Vehicle.TrafficNeon"), &NeonData::m_bApplyOnTraffic, TEXT("Vehicle.TrafficNeonMSG"));
                    ImGui::Columns(1);

                    ImGui::Spacing();

                    if (ImGui::ColorEdit3(TEXT("Vehicle.ColorPicker"), NeonData::m_fColorPicker))
                    {
                        int r = static_cast<int>(NeonData::m_fColorPicker[0] * 255);
                        int g = static_cast<int>(NeonData::m_fColorPicker[1] * 255);
                        int b = static_cast<int>(NeonData::m_fColorPicker[2] * 255);

                        Neon::Install(veh, r, g, b);
                    }


                    ImGui::Spacing();
                    ImGui::Text(TEXT("Vehicle.SelectPreset"));

                    int count = (int)m_CarcolsColorData.size();
                    ImVec2 size = Widget::CalcSize();
                    int btnsInRow = ImGui::GetWindowContentRegionWidth() / (size.y * 2);
                    int btnSize = (ImGui::GetWindowContentRegionWidth() - int(ImGuiStyleVar_ItemSpacing) * (btnsInRow -
                                   0.6 * btnsInRow)) / btnsInRow;

                    ImGui::BeginChild("Neonss");

                    for (int color_id = 0; color_id < count; ++color_id)
                    {
                        auto& color = m_CarcolsColorData[color_id];
                        if (Widget::ColorBtn(color_id, color, ImVec2(btnSize, btnSize)))
                        {
                            int r = static_cast<int>(color[0] * 255);
                            int g = static_cast<int>(color[1] * 255);
                            int b = static_cast<int>(color[2] * 255);

                            Neon::Install(veh, r, g, b);
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
                    Paint::GenerateNodeList(veh, PaintData::m_vecNames, PaintData::m_Selected);

                    ImGui::Spacing();
                    if (ImGui::Button(TEXT("Vehicle.ResetTexture"), ImVec2(Widget::CalcSize())))
                    {
                        Paint::ResetNodeTexture(veh, PaintData::m_Selected);
                        Util::SetMessage(TEXT("Vehicle.ResetTextureMSG"));
                    }
                    ImGui::Spacing();

                    Widget::ListBox(TEXT("Vehicle.Component"), PaintData::m_vecNames, PaintData::m_Selected);
                    ImGui::Spacing();

                    ImGui::Columns(2, NULL, false);
                    ImGui::Checkbox(TEXT("Vehicle.MatFilter"), &PaintData::m_bMatFilter);
                    ImGui::NextColumn();
                    int maxpjob, curpjob;
                    Command<Commands::GET_NUM_AVAILABLE_PAINTJOBS>(hveh, &maxpjob);

                    if (maxpjob > 0)
                    {
                        Command<Commands::GET_CURRENT_VEHICLE_PAINTJOB>(hveh, &curpjob);
                        
                        if (ImGui::ArrowButton("Left", ImGuiDir_Left))
                        {
                            curpjob -= 1;
                            if (curpjob < -1)
                            {
                               curpjob = maxpjob - 1; 
                            }
                            Command<Commands::GIVE_VEHICLE_PAINTJOB>(hveh, curpjob);
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
                            Command<Commands::GIVE_VEHICLE_PAINTJOB>(hveh, curpjob);
                        }

                        ImGui::Spacing();
                    }
                    ImGui::Columns(1);
                    ImGui::Spacing();
                    Widget::ImageList(Paint::m_TextureData,
                                   [](std::string& str)
                    {
                        Paint::SetNodeTexture(FindPlayerPed()->m_pVehicle, PaintData::m_Selected, str,
                                              PaintData::m_bMatFilter);
                    },
                    [](std::string& str)
                    {
                        return str;
                    });

                    ImGui::EndTabItem();
                }
            }
            if (ImGui::BeginTabItem(TEXT("Vehicle.TuneTab")))
            {
                ImGui::Spacing();
                Widget::ImageList(m_TuneData,
                               [](std::string& str)
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
                CBaseModelInfo* pInfo = CModelInfo::GetModelInfo(pPlayer->m_pVehicle->m_nModelIndex);
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
#endif
        }
        ImGui::EndTabBar();
    }
}