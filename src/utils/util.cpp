#include "pch.h"
#include <math.h>
#include <psapi.h>
#include "util.h"
#include <CPopulation.h>

ImVec2 Util::ConvertScreenToMap(ImVec2 pos, ImVec2 mapSz, ImVec2 screenSz)
{
    float size = screenSz.x * screenSz.y / screenSz.x;
    float left = (screenSz.x-size) / 2;

    pos.x -= left;
    pos.x -= size/2;
    pos.y -= size/2;

    // Convert image space to map space
    pos.x = pos.x / size * mapSz.x;
    pos.y = pos.y / size * mapSz.y;
    pos.y *= -1;

    return pos;
}

ImVec2 Util::ConvertMapToScreen(ImVec2 pos, ImVec2 mapSz, ImVec2 screenSz)
{
    float size = screenSz.x * screenSz.y / screenSz.x;
    float left = (screenSz.x-size) / 2;

    // Convert map space to screen space
    pos.x = pos.x / mapSz.x * size;
    pos.y = pos.y / mapSz.y * size;
    pos.y *= -1;

    pos.x += left;
    pos.x += size/2;
    pos.y += size/2;

    return pos;
}

std::string Util::GetCarName(int model)
{
#ifdef GTA3
    return std::to_string(model);
#else
    return (const char*)CModelInfo::GetModelInfo(model) + 0x32;
#endif
}

int Util::GetCarModel(const char* name)
{
    int model = 0;
    CBaseModelInfo* pModelInfo = CModelInfo::GetModelInfo((char*)name, &model);

    if (model > 0 && model < 1000000 && GetCarName(model) != "")
    {
        return model;
    }
    else
    {
        return 0;
    }
}

void Util::SetMessage(const char *message, bool b1, bool b2, bool b3)
{
#if GTASA
    CHud::SetHelpMessage(message, b1, b2, b3);
#elif GTAVC
    CHud::SetHelpMessage(message, b1, b2);
#else
    const size_t size = strlen(message)+1;
    wchar_t* wc = new wchar_t[size];
    mbstowcs(wc, message, size);
    CHud::SetHelpMessage(wc, b1);
    delete wc;
#endif
}

float Util::RoundFloat(float val)
{
    return roundf(val * 100) / 100;
}

bool Util::IsInCar(CPed *pPed)
{
    if (!pPed)
    {
        return false;
    }

    return BY_GAME(pPed->m_nPedFlags.bInVehicle, pPed->m_bInVehicle, pPed->m_bInVehicle);
}

void Util::FixCar(CVehicle *pVeh)
{
#ifdef GTASA
    pVeh->Fix();
#else
    switch (pVeh->m_nVehicleClass)
    {
    case VEHICLE_AUTOMOBILE:
    {
        reinterpret_cast<CAutomobile*>(pVeh)->Fix();
        break;
    }
#ifdef GTAVC
    case VEHICLE_BIKE:
    {
        reinterpret_cast<CBike *>(pVeh)->Fix();
        break;
    }
#endif
    }
#endif
    pVeh->m_fHealth = 1000.0f;
}

void Util::UnFlipCar(CVehicle *pVeh)
{
#ifdef GTASA
    int hveh = CPools::GetVehicleRef(pVeh);
    float roll;

    Command<Commands::GET_CAR_ROLL>(hveh, &roll);
    roll += 180;
    Command<Commands::SET_CAR_ROLL>(hveh, roll);
    Command<Commands::SET_CAR_ROLL>(hveh, roll); // z rot fix
#elif GTAVC
    float x,y,z;
    pVeh->m_placement.GetOrientation(x, y, z);
    y += 135.0f;
    pVeh->m_placement.SetOrientation(x, y, z);
#else
    float x,y,z;
    pVeh->GetOrientation(x, y, z);
    y += 135.0f;
    pVeh->SetOrientation(x, y, z);
#endif
}

void Util::SetCarForwardSpeed(CVehicle *pVeh, float speed)
{
#ifdef GTA3
    CVector inVec = pVeh->m_matrix.up;
    double factor = speed / 60.0;
    pVeh->m_vecMoveSpeed.x = factor * inVec.x;
    pVeh->m_vecMoveSpeed.y = factor * inVec.y;
    pVeh->m_vecMoveSpeed.z = factor * inVec.z;
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
void Util::ClearCharTasksCarCheck(CPed* pPed)
{
    uint hped = CPools::GetPedRef(pPed);
    uint hveh = NULL;
    bool veh_engine = true;
    float speed;

    if (IsInCar(pPed))
    {
        hveh = CPools::GetVehicleRef(pPed->m_pVehicle);
        veh_engine = pPed->m_pVehicle->m_nVehicleFlags.bEngineOn;
        speed = pPed->m_pVehicle->m_vecMoveSpeed.Magnitude() * 50.0f;
    }

    Command<Commands::CLEAR_CHAR_TASKS_IMMEDIATELY>(hped);

    if (hveh)
    {
        Command<Commands::TASK_WARP_CHAR_INTO_CAR_AS_DRIVER>(hped, hveh);
        pPed->m_pVehicle->m_nVehicleFlags.bEngineOn = veh_engine;
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
CVehicle* Util::GetClosestCar()
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
