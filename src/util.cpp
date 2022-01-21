#include "pch.h"
#include "util.h"
#include "psapi.h"

void Util::SetCarForwardSpeed(CVehicle *pVeh, float speed)
{
#ifdef GTA3
    CVector inVec = pVeh->m_matrix.up;
    double speedFactor = speed * 0.016766668;
    pVeh->m_vecMoveSpeed.x = speedFactor * inVec.x;
    pVeh->m_vecMoveSpeed.y = speedFactor * inVec.y;
    pVeh->m_vecMoveSpeed.z = speedFactor * inVec.z;
#else
    Command<Commands::SET_CAR_FORWARD_SPEED>(CPools::GetVehicleRef(pVeh), speed);
#endif
}

std::string Util::GetLocationName(CVector* pos)
{
#ifdef GTASA
    CPlayerPed  *pPlayer = FindPlayerPed();
    int hplayer = CPools::GetPedRef(pPlayer);

    int interior = 0;
    Command<Commands::GET_AREA_VISIBLE>(&interior);

    std::string town = "San Andreas";
    int city;
    Command<Commands::GET_CITY_PLAYER_IS_IN>(&hplayer, &city);

    switch (city)
    {
    case 0:
        town = "CS";
        break;
    case 1:
        town = "LS";
        break;
    case 2:
        town = "SF";
        break;
    case 3:
        town = "LV";
        break;
    }

    if (interior == 0)
    {
        return CTheZones::FindSmallestZoneForPosition(*pos, true)->GetTranslatedName() + std::string(", ") + town;
    }
    return std::string("Interior ") + std::to_string(interior) + ", " + town;

#elif GTAVC
    return "Vice City";
#else
    return "Liberty City";
#endif
}

#ifdef GTASA
void Util::ClearCharTasksVehCheck(CPed* ped)
{
    uint hped = CPools::GetPedRef(ped);
    uint hveh = NULL;
    bool veh_engine = true;
    float speed;

    if (ped->m_nPedFlags.bInVehicle)
    {
        hveh = CPools::GetVehicleRef(ped->m_pVehicle);
        veh_engine = ped->m_pVehicle->m_nVehicleFlags.bEngineOn;
        speed = ped->m_pVehicle->m_vecMoveSpeed.Magnitude() * 50.0f;
    }

    Command<Commands::CLEAR_CHAR_TASKS_IMMEDIATELY>(hped);

    if (hveh)
    {
        Command<Commands::TASK_WARP_CHAR_INTO_CAR_AS_DRIVER>(hped, hveh);
        ped->m_pVehicle->m_nVehicleFlags.bEngineOn = veh_engine;
        Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, speed);
    }
}

bool Util::IsOnMission()
{
    return FindPlayerPed()->CanPlayerStartMission() && !*(patch::Get<char*>(0x5D5380, false) + CTheScripts::OnAMissionFlag);
}

int Util::GetLargestGangInZone()
{
    int gang_id = 0, max_density = 0;

    for (int i = 0; i != 10; ++i)
    {
        CVector pos = FindPlayerPed()->GetPosition();

        CZoneInfo* zone_info = CTheZones::GetZoneInfo(&pos, nullptr);
        int density = zone_info->m_nGangDensity[i];

        if (density > max_density)
        {
            max_density = density;
            gang_id = i;
        }
    }

    return gang_id;
}
#endif

// implemention of opcode 0AB5 (STORE_CLOSEST_ENTITIES)
// https://github.com/cleolibrary/CLEO4/blob/916d400f4a731ba1dd0ff16e52bdb056f42b7038/source/CCustomOpcodeSystem.cpp#L1671
CVehicle* Util::GetClosestVehicle()
{
    CPlayerPed* player = FindPlayerPed();

#ifdef GTASA
    CPedIntelligence* pedintel;
    if (player && (pedintel = player->m_pIntelligence))
    {
        CVehicle* veh = nullptr;
        for (int i = 0; i < 16; i++)
        {
            veh = static_cast<CVehicle*>(pedintel->m_vehicleScanner.m_apEntities[i]);
            if (veh && !veh->m_nVehicleFlags.bFadeOut)
                break;
            veh = nullptr;
        }

        return veh;
    }
    return nullptr;
#else

    CVehicle *pClosestVeh = nullptr;
    float distance = 999.0f;

    CVector playerPos = player->GetPosition();

    for (CVehicle *pVeh : CPools::ms_pVehiclePool)
    {
        CVector pos = pVeh->GetPosition();
        float dist = DistanceBetweenPoints(playerPos, pos);

        if (dist < distance)
        {
            pClosestVeh = pVeh;
            distance = dist;
        }
    }
    return pClosestVeh;
#endif
}

CPed* Util::GetClosestPed()
{
    CPlayerPed* player = FindPlayerPed();
#ifdef GTASA
    CPedIntelligence* pedintel;
    if (player && (pedintel = player->m_pIntelligence))
    {
        CPed* ped = nullptr;

        for (int i = 0; i < 16; i++)
        {
            ped = static_cast<CPed*>(pedintel->m_pedScanner.m_apEntities[i]);
            if (ped && ped != player && (ped->m_nCreatedBy & 0xFF) == 1 && !ped->m_nPedFlags.bFadeOut)
                break;
            ped = nullptr;
        }

        return ped;
    }
    return nullptr;
#else
    return player->m_apNearPeds[0];
#endif
}

bool Util::IsOnCutscene()
{
    return BY_GAME(CCutsceneMgr::ms_running, *(bool*)0xA10AB2, *(bool*)0x95CCF5);
}

void Util::RainbowValues(int& r, int& g, int& b, float speed)
{
    int timer = CTimer::m_snTimeInMilliseconds / 150;
    r = sin(timer * speed) * 127 + 128;
    g = sin(timer * speed + 2) * 127 + 128;
    b = sin(timer * speed + 4) * 127 + 128;
}

void Util::GetCPUUsageInit()
{
    PdhOpenQuery(nullptr, NULL, &cpuQuery);
    PdhAddEnglishCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
}

double Util::GetCurrentCPUUsage()
{
    PDH_FMT_COUNTERVALUE counterVal;

    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, nullptr, &counterVal);
    return counterVal.doubleValue;
}
