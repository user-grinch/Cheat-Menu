#include "pch.h"
#include "Util.h"
#include "psapi.h"
#include "CFileLoader.h"

void Util::LoadTextureDirectory(SSearchData& data, char *path, bool pass_full_name)
{
	RwTexDictionary* pRwTexDictionary = &data.txd;

	pRwTexDictionary = CFileLoader::LoadTexDictionary(path);

	if (pRwTexDictionary) 
	{
		// FIX ME
		if (pass_full_name)
		{
			RwTexDictionaryForAllTextures(pRwTexDictionary, [](RwTexture* tex, void* data)
			{
				SSearchData* sdata = reinterpret_cast<SSearchData*>(data);
				sdata->m_ImagesList.push_back(std::make_unique<STextureStructure>());
				sdata->m_ImagesList.back().get()->m_pRwTexture = tex;

				std::stringstream ss(tex->name);
				std::string str;
				getline(ss, str, '$');

				sdata->m_ImagesList.back().get()->m_CategoryName = str;
				if (!std::count(sdata->m_Categories.begin(), sdata->m_Categories.end(), str))
				{
					sdata->m_Categories.push_back(str);
				}
				sdata->m_ImagesList.back().get()->m_FileName = tex->name;

				return tex;
			}, &data);
		}
		else
		{
			RwTexDictionaryForAllTextures(pRwTexDictionary, [](RwTexture* tex, void* data)
			{
				SSearchData* sdata = reinterpret_cast<SSearchData*>(data);
				sdata->m_ImagesList.push_back(std::make_unique<STextureStructure>());
				sdata->m_ImagesList.back().get()->m_pRwTexture = tex;

				std::stringstream ss(tex->name);
				std::string str;
				getline(ss, str, '$');

				sdata->m_ImagesList.back().get()->m_CategoryName = str;
				if (!std::count(sdata->m_Categories.begin(), sdata->m_Categories.end(), str))
				{
					sdata->m_Categories.push_back(str);
				}
				getline(ss, str);
				sdata->m_ImagesList.back().get()->m_FileName = str;

				return tex;
			}, &data);
		}
	}
}

void* Util::GetTextureFromRaster(RwTexture *pTexture)
{
	RwRasterEx* raster = (RwRasterEx*)(&pTexture->raster->parent);
	
	return (&raster->renderResource->texture);
}

std::string Util::GetLocationName(CVector* pos)
{
	#ifdef GTASA
	int hplayer = CPools::GetPedRef(FindPlayerPed());

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
		return CTheZones::FindSmallestZoneForPosition(*pos, true)->GetTranslatedName() + std::string(", ") + town;
	return std::string("Interior ") + std::to_string(interior) + ", " + town;
	
	#elif GTAVC
	return "Vice City";
	#endif
}

#ifdef GTASA
// Thanks DKPac22
RwTexture* Util::LoadTextureFromMemory(char* data, unsigned int size)
{
	patch::SetChar(0x7CF9CA, rwSTREAMMEMORY);
	RwMemory memoryImage;
	RwInt32 width, height, depth, flags;
	memoryImage.start = (RwUInt8*)data;
	memoryImage.length = size;
	RwImage* image = RtPNGImageRead((char*)&memoryImage);
	RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
	RwRaster* raster = RwRasterCreate(width, height, depth, flags);
	RwRasterSetFromImage(raster, image);
	RwImageDestroy(image);
	patch::SetChar(0x7CF9CA, rwSTREAMFILENAME);

	return RwTextureCreate(raster);
}

void Util::ClearCharTasksVehCheck(CPed* ped)
{
	uint hped = CPools::GetPedRef(ped);
	uint hveh = NULL;
	bool veh_engine = true;

	if (ped->m_nPedFlags.bInVehicle)
	{
		hveh = CPools::GetVehicleRef(ped->m_pVehicle);
		veh_engine = ped->m_pVehicle->m_nVehicleFlags.bEngineOn;
	}

	Command<Commands::CLEAR_CHAR_TASKS_IMMEDIATELY>(hped);

	if (hveh)
	{
		Command<Commands::TASK_WARP_CHAR_INTO_CAR_AS_DRIVER>(hped, hveh);
		ped->m_pVehicle->m_nVehicleFlags.bEngineOn = veh_engine;
	}
}

bool Util::IsOnMission()
{
	return FindPlayerPed()->CanPlayerStartMission() && !*(patch::Get<char*>(0x5D5380, false) +
		CTheScripts::OnAMissionFlag);
}

bool Util::IsOnCutscene()
{
	return CCutsceneMgr::ms_running;
}

int Util::GetLargestGangInZone()
{
	int gang_id = 0, max_density = 0;

	for (int i = 0; i != 10; ++i)
	{
		CVector pos = FindPlayerPed()->GetPosition();

		CZoneExtraInfo* zone_info = CTheZones::GetZoneInfo(&pos, nullptr);
		int density = zone_info->m_nGangDensity[i];
		
		if (density > max_density)
		{
			max_density = density;
			gang_id = i;
		}
	}

	return gang_id;
}

// implemention of opcode 0AB5 (STORE_CLOSEST_ENTITIES)
// https://github.com/cleolibrary/CLEO4/blob/916d400f4a731ba1dd0ff16e52bdb056f42b7038/source/CCustomOpcodeSystem.cpp#L1671
CVehicle* Util::GetClosestVehicle()
{
	CPlayerPed* player = FindPlayerPed();
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
}

CPed* Util::GetClosestPed()
{
	CPlayerPed* player = FindPlayerPed();
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
}
#endif	

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
