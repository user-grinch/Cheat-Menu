#include "pch.h"
#include "Util.h"
#include "external/imgui/stb_image.h"

void Util::ClearCharTasksVehCheck(CPed* ped)
{
	uint hped = CPools::GetPedRef(ped);
	uint hveh = NULL;
	bool veh_engine = true;

	if (Command<Commands::IS_CHAR_IN_ANY_CAR>(hped))
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

void Util::LoadTexturesInDirRecursive(const char *path, const char *file_ext,std::vector<std::string>& category_vec, std::vector<std::unique_ptr<TextureStructure>> &store_vec)
{
	std::string folder = "";
	for (auto &p : std::experimental::filesystem::recursive_directory_iterator(path))
	{
		
		if (p.path().extension() == file_ext)
		{
			store_vec.push_back(std::make_unique<TextureStructure>());
			HRESULT hr = -1;
			if (Globals::renderer == Render_DirectX9)
				hr = D3DXCreateTextureFromFileA(GetD3DDevice(), p.path().string().c_str(), &store_vec.back().get()->texture9);

			if (Globals::renderer == Render_DirectX11)
			{
				if (LoadTextureFromFileDx11(p.path().string().c_str(), &store_vec.back().get()->texture11))
					hr = S_OK;
			}


			if (hr == S_OK)
			{
				store_vec.back().get()->file_name = p.path().stem().string();
				store_vec.back().get()->category_name = folder;
			}
			else
			{
				flog << "Couldn't load image " << p.path().stem().string() << std::endl;
				store_vec.pop_back();
			}
		}
		else
		{
			folder = p.path().stem().string();
			category_vec.push_back(folder);
		}
	}
}

bool Util::LoadTextureFromFileDx11(const char* filename, ID3D11ShaderResourceView** out_srv)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D *pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;

	Globals::device11->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Globals::device11->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	stbi_image_free(image_data);

	return true;
}

bool Util::IsOnMission()
{
	return FindPlayerPed()->CanPlayerStartMission() && !*(patch::Get<char*>(0x5D5380, false) + CTheScripts::OnAMissionFlag);
}

std::string Util::GetLocationName(CVector *pos)
{
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
	else
		return std::string("Interior ") + std::to_string(interior) + ", " + town;

}

int Util::GetLargestGangInZone()
{
	int gang_id = 0, max_density = 0;

	for (int i = 0; i != 10; ++i)
	{
		CVector pos = FindPlayerPed()->GetPosition();
		CZone *zone = &CZone();
		
		CZoneExtraInfo *zone_info = CTheZones::GetZoneInfo(&pos, &zone);
		int density = zone_info->m_nGangDensity[i];

		if (density > max_density)
		{
			max_density = density;
			gang_id = i;
		}
	}

	return gang_id;
}

// partial implemention of opcode 0AB5 (STORE_CLOSEST_ENTITIES)
// https://github.com/cleolibrary/CLEO4/blob/916d400f4a731ba1dd0ff16e52bdb056f42b7038/source/CCustomOpcodeSystem.cpp#L1671
CVehicle* Util::GetClosestVehicle(CPlayerPed* player)
{
	CPedIntelligence *pedintel;
	if (player && (pedintel = player->m_pIntelligence))
	{
		CVehicle *veh = nullptr;
		for (int i = 0; i < 16; i++)
		{
			veh = (CVehicle*)pedintel->m_vehicleScanner.m_apEntities[i];
			if (veh && !veh->m_nVehicleFlags.bFadeOut)
				break;
			veh = nullptr;
		}

		return veh;
	}
	return nullptr;
}

void Util::RainbowValues(int &r, int&g, int &b, float speed)
{
	int timer = CTimer::m_snTimeInMilliseconds/150;
	r = sin(timer * speed) * 127 + 128;
	g = sin(timer * speed + 2) * 127 + 128;
	b = sin(timer * speed + 4) * 127 + 128;
}